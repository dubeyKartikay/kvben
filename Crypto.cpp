#include "Crypto.hpp"

#include <fstream>
#include <iomanip>
#include <memory> // For std::unique_ptr
#include <sstream>
#include <vector>

// --- OpenSSL Headers ---
#include <openssl/err.h>
#include <openssl/evp.h>

// --- Anonymous Namespace for Internal Helpers ---
// This restricts the visibility of these helpers to this translation unit only,
// effectively avoiding global variables/functions.
namespace {

// Buffer size for reading file chunks
const size_t HASH_BUFFER_SIZE = 4096; // 4 KB

// Helper function to get OpenSSL error string
std::string get_openssl_error() {
  unsigned long err_code = ERR_get_error();
  if (err_code == 0) {
    return "No OpenSSL error code reported"; // Provide a default message
  }
  char err_buf[256];
  ERR_error_string_n(err_code, err_buf, sizeof(err_buf));
  ERR_clear_error(); // Clear the error queue
  return std::string(err_buf);
}

// Custom Deleter for EVP_MD_CTX using unique_ptr for RAII
struct EvpMdCtxDeleter {
  void operator()(EVP_MD_CTX *ctx) const {
    if (ctx) {
      EVP_MD_CTX_free(ctx);
    }
  }
};
using EvpMdCtxPtr = std::unique_ptr<EVP_MD_CTX, EvpMdCtxDeleter>;

// Helper function to convert raw hash bytes to a hex string
std::string bytes_to_hex_string(const unsigned char *bytes, size_t len) {
  std::stringstream hex_stream;
  hex_stream << std::hex << std::setfill('0');
  for (size_t i = 0; i < len; ++i) {
    hex_stream << std::setw(2) << static_cast<int>(bytes[i]);
  }
  return hex_stream.str();
}

// Core hashing logic using EVP - takes an initialized context and data source
// Templated to handle both ifstream and a simple data pointer
template <typename DataSource>
void perform_hash_update(EVP_MD_CTX *ctx, DataSource &source) {
  // Overload for file streams
  if constexpr (std::is_same_v<DataSource, std::ifstream>) {
    std::vector<char> buffer(HASH_BUFFER_SIZE);
    while (source.good()) {
      source.read(buffer.data(), buffer.size());
      std::streamsize bytes_read = source.gcount();

      if (bytes_read > 0) {
        if (1 != EVP_DigestUpdate(ctx, buffer.data(),
                                  static_cast<size_t>(bytes_read))) {
          throw CryptoError("EVP_DigestUpdate failed: " + get_openssl_error());
        }
      }
    }
    // Check for stream errors *after* the loop
    if (source.bad()) {
      throw CryptoError("Error reading file stream");
    }
  }
  // Overload for raw data (like a string's buffer)
  else if constexpr (std::is_pointer_v<DataSource> &&
                     std::is_same_v<std::remove_pointer_t<DataSource>,
                                    const char>) {
    // This overload isn't strictly needed for the current API,
    // but shows how you might generalize if you had a function
    // taking raw buffers. For the string version, we'll just call
    // EVP_DigestUpdate directly with string::data() and string::length().
    // Let's simplify and remove this template for now, handling
    // string and file separately in the public methods is clearer.
  }
}

} // End anonymous namespace

// --- Public Static Method Implementations ---

std::string Crypto::getSHA256Hash(const std::filesystem::path &filename) {
  // 1. Get Algorithm
  const EVP_MD *md = EVP_sha256();
  if (!md) {
    // This is highly unlikely unless OpenSSL is severely broken
    throw CryptoError(
        "Failed to get SHA256 algorithm (EVP_sha256). OpenSSL init issue?");
  }

  // 2. Create and Initialize Context (RAII)
  EvpMdCtxPtr mdctx(EVP_MD_CTX_new());
  if (!mdctx) {
    throw CryptoError("Failed to create EVP_MD_CTX: " + get_openssl_error());
  }
  if (1 != EVP_DigestInit_ex(mdctx.get(), md, NULL)) {
    throw CryptoError("Failed to initialize EVP digest: " +
                      get_openssl_error());
  }

  // 3. Open File (handle potential filesystem errors before this)
  std::ifstream file(filename, std::ios::binary);
  if (!file.is_open()) {
    throw CryptoError("Cannot open file: " + filename.string());
  }

  // 4. Read File and Update Hash
  std::vector<char> buffer(
      HASH_BUFFER_SIZE); // Use constant from anon namespace
  while (file.good()) {
    file.read(buffer.data(), buffer.size());
    std::streamsize bytes_read = file.gcount();

    if (bytes_read > 0) {
      if (1 != EVP_DigestUpdate(mdctx.get(), buffer.data(),
                                static_cast<size_t>(bytes_read))) {
        // mdctx unique_ptr will clean up context automatically
        throw CryptoError("EVP_DigestUpdate failed: " + get_openssl_error());
      }
    }
  }

  // Check for read errors *after* the loop
  if (file.bad()) {
    // file closes automatically, mdctx cleans up automatically
    throw CryptoError("Error reading file: " + filename.string());
  }
  // file is closed automatically when it goes out of scope here

  // 5. Finalize Hash
  unsigned char hash_result[EVP_MAX_MD_SIZE];
  unsigned int hash_len = 0;
  if (1 != EVP_DigestFinal_ex(mdctx.get(), hash_result, &hash_len)) {
    // mdctx cleans up automatically
    throw CryptoError("EVP_DigestFinal_ex failed: " + get_openssl_error());
  }

  // mdctx is freed automatically by unique_ptr when it goes out of scope here

  // 6. Convert to Hex String
  return bytes_to_hex_string(hash_result, hash_len); // Use helper
}

std::string Crypto::getSHA256Hash(const std::string &input) {
  // 1. Get Algorithm
  const EVP_MD *md = EVP_sha256();
  if (!md) {
    throw CryptoError(
        "Failed to get SHA256 algorithm (EVP_sha256). OpenSSL init issue?");
  }

  // 2. Create and Initialize Context (RAII)
  EvpMdCtxPtr mdctx(EVP_MD_CTX_new());
  if (!mdctx) {
    throw CryptoError("Failed to create EVP_MD_CTX: " + get_openssl_error());
  }
  if (1 != EVP_DigestInit_ex(mdctx.get(), md, NULL)) {
    throw CryptoError("Failed to initialize EVP digest: " +
                      get_openssl_error());
  }

  // 3. Update Hash with String Data (Single Update)
  if (!input.empty()) { // Handle empty string case correctly
    if (1 != EVP_DigestUpdate(mdctx.get(), input.data(), input.length())) {
      throw CryptoError("EVP_DigestUpdate failed for string: " +
                        get_openssl_error());
    }
  }

  // 4. Finalize Hash
  unsigned char hash_result[EVP_MAX_MD_SIZE];
  unsigned int hash_len = 0;
  if (1 != EVP_DigestFinal_ex(mdctx.get(), hash_result, &hash_len)) {
    throw CryptoError("EVP_DigestFinal_ex failed for string: " +
                      get_openssl_error());
  }

  // mdctx is freed automatically by unique_ptr

  // 5. Convert to Hex String
  return bytes_to_hex_string(hash_result, hash_len); // Use helper
}
