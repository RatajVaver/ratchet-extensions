#include "RatchetExtension.h"

#include <windows.h>
#include <bcrypt.h>
#include <string>
#include <vector>

static std::string bcrypt_hash(const LPCWSTR algorithm, const BYTE* data, const size_t size) {
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

    static constexpr char hex[] = "0123456789abcdef";
    std::string out(cbHash * 2, 0);
    for (DWORD i = 0; i < cbHash; ++i) {
        out[i * 2] = hex[hash[i] >> 4];
        out[i * 2 + 1] = hex[hash[i] & 0xF];
    }
    return out;
}

static int hash(lua_State* L, const LPCWSTR algo) {
    size_t len;
    const auto* s = reinterpret_cast<const BYTE*>(luaL_checklstring(L, 1, &len));
    const std::string result = bcrypt_hash(algo, s, len);
    if (result.empty()) return luaL_error(L, "hashing failed");
    lua_pushstring(L, result.c_str());
    return 1;
}

static int md5(lua_State* L) { return hash(L, BCRYPT_MD5_ALGORITHM); }
static int sha1(lua_State* L) { return hash(L, BCRYPT_SHA1_ALGORITHM); }
static int sha256(lua_State* L) { return hash(L, BCRYPT_SHA256_ALGORITHM); }
static int sha512(lua_State* L) { return hash(L, BCRYPT_SHA512_ALGORITHM); }

extern "C" __declspec(dllexport) void RatchetRegister(const RatchetAPI* api) {
    api->register_ext("Hash", "1.0.0");

    lua_State* L = api->L;
    lua_register(L, "md5", md5);
    lua_register(L, "sha1", sha1);
    lua_register(L, "sha256", sha256);
    lua_register(L, "sha512", sha512);
}
