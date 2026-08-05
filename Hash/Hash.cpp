#include "RatchetExtension.h"

#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <bcrypt.h>
#else
#include <openssl/evp.h>
#endif

static constexpr char hex[] = "0123456789abcdef";

static std::string to_hex(const unsigned char* data, const size_t size) {
    std::string out(size * 2, 0);
    for (size_t i = 0; i < size; ++i) {
        out[i * 2] = hex[data[i] >> 4];
        out[i * 2 + 1] = hex[data[i] & 0xF];
    }
    return out;
}

#ifdef _WIN32
static std::string do_hash(const LPCWSTR algorithm, const BYTE* data, const size_t size) {
    BCRYPT_ALG_HANDLE hAlg = nullptr;
    if (!BCRYPT_SUCCESS(BCryptOpenAlgorithmProvider(&hAlg, algorithm, nullptr, 0)))
        return "";

    DWORD cbObj = 0, cbHash = 0, cbResult = 0;
    (void) BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PUCHAR>(&cbObj), sizeof(cbObj), &cbResult, 0);
    (void) BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH, reinterpret_cast<PUCHAR>(&cbHash), sizeof(cbHash), &cbResult, 0);

    std::vector<BYTE> obj(cbObj), hash(cbHash);

    BCRYPT_HASH_HANDLE hHash = nullptr;
    (void) BCryptCreateHash(hAlg, &hHash, obj.data(), cbObj, nullptr, 0, 0);
    (void) BCryptHashData(hHash, const_cast<BYTE*>(data), static_cast<ULONG>(size), 0);
    (void) BCryptFinishHash(hHash, hash.data(), cbHash, 0);
    (void) BCryptDestroyHash(hHash);
    (void) BCryptCloseAlgorithmProvider(hAlg, 0);

    return to_hex(hash.data(), cbHash);
}
#else
static std::string do_hash(const EVP_MD* algorithm, const unsigned char* data, const size_t size) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) return "";

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen = 0;
    const bool ok = EVP_DigestInit_ex(ctx, algorithm, nullptr) == 1
                    && EVP_DigestUpdate(ctx, data, size) == 1
                    && EVP_DigestFinal_ex(ctx, hash, &hashLen) == 1;
    EVP_MD_CTX_free(ctx);

    return ok ? to_hex(hash, hashLen) : "";
}
#endif

#ifdef _WIN32
using HashAlgo = LPCWSTR;
#else
using HashAlgo = const EVP_MD*;
#endif

static int hash(lua_State* L, const HashAlgo algo) {
    size_t len;
    const auto* s = reinterpret_cast<const unsigned char*>(luaL_checklstring(L, 1, &len));
    const std::string result = do_hash(algo, s, len);
    if (result.empty()) return luaL_error(L, "hashing failed");
    lua_pushstring(L, result.c_str());
    return 1;
}

#ifdef _WIN32
static int md5(lua_State* L) { return hash(L, BCRYPT_MD5_ALGORITHM); }
static int sha1(lua_State* L) { return hash(L, BCRYPT_SHA1_ALGORITHM); }
static int sha256(lua_State* L) { return hash(L, BCRYPT_SHA256_ALGORITHM); }
static int sha512(lua_State* L) { return hash(L, BCRYPT_SHA512_ALGORITHM); }
#else
static int md5(lua_State* L) { return hash(L, EVP_md5()); }
static int sha1(lua_State* L) { return hash(L, EVP_sha1()); }
static int sha256(lua_State* L) { return hash(L, EVP_sha256()); }
static int sha512(lua_State* L) { return hash(L, EVP_sha512()); }
#endif

RATCHET_EXTENSION_EXPORT void RatchetRegister(const RatchetAPI* api) {
    api->register_ext("Hash", "1.0.0");

    lua_State* L = api->L;
    lua_register(L, "md5", md5);
    lua_register(L, "sha1", sha1);
    lua_register(L, "sha256", sha256);
    lua_register(L, "sha512", sha512);
}
