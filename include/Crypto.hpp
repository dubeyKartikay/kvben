#pragma once
#include <filesystem> // Required for std::filesystem::path
#include <stdexcept>  // For custom exception
#include <string>

class CryptoError : public std::runtime_error {
public:
  using std::runtime_error::runtime_error; // Inherit constructors
};

class Crypto {
public:
  Crypto() = delete;
  ~Crypto() = delete;
  Crypto(const Crypto &) = delete;
  Crypto &operator=(const Crypto &) = delete;
  Crypto(Crypto &&) = delete;
  Crypto &operator=(Crypto &&) = delete;

  /**
   * @brief Calculates the SHA-256 hash of a file.
   * @param filename Path to the file.
   * @return The SHA-256 hash as a lowercase hexadecimal string.
   * @throws CryptoError if the file cannot be opened, read, or if an OpenSSL
   * error occurs.
   * @throws std::filesystem::filesystem_error for filesystem-related issues
   * before opening.
   */
  static std::string getSHA256Hash(const std::filesystem::path &filename);

  /**
   * @brief Calculates the SHA-256 hash of a string.
   * @param input The input string.
   * @return The SHA-256 hash as a lowercase hexadecimal string.
   * @throws CryptoError if an OpenSSL error occurs.
   */
  static std::string getSHA256Hash(const std::string &input);

private:
};
