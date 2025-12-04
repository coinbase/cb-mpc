#include <memory>
#include <vector>

#include <cbmpc/crypto/commitment.h>
#include <cbmpc/crypto/ro.h>
#include <cbmpc/crypto/base_pki.h>

bn_t hash_number()
{
  ecurve_t c = coinbase::crypto::curve_p256;
  const mod_t& q = c.order();
  bn_t r = bn_t::rand(q);
  ecc_point_t G = c.generator();

  return coinbase::crypto::ro::hash_number(c, G, r, 42).mod(q);
}

mem_t hash_string()
{
  ecurve_t c = coinbase::crypto::curve_p256;
  const mod_t& q = c.order();
  bn_t r = bn_t::rand(q);
  ecc_point_t G = c.generator();

  return coinbase::crypto::ro::hash_string(c, G, r, 42).bitlen(256).take(32);
}

ecc_point_t hash_curve()
{
  ecurve_t c = coinbase::crypto::curve_p256;
  const mod_t& q = c.order();
  bn_t r = bn_t::rand(q);
  ecc_point_t G = c.generator();

  return coinbase::crypto::ro::hash_curve(c, G, r, 42).curve(coinbase::crypto::curve_p256);
}

error_t com()
{
  ecurve_t c = coinbase::crypto::curve_p256;
  ecc_point_t G = c.generator();
  buf_t sid = coinbase::crypto::gen_random(16);

  pid_t pid = coinbase::crypto::pid_from_name("test");
  coinbase::crypto::commitment_t com(sid, pid);
  com.gen(G);
  std::cout << bn_t(com.msg).to_string() << std::endl;
  return com.open(G);
}

int main(int argc, const char* argv[])
{
  std::cout << "================ hash ===============\n";
  std::cout << "hash_string() = " << hash_string().to_string() << "\n";
  std::cout << "hash_number() = " << hash_number().to_string() << "\n";
  std::cout << "hash_curve() = " << hash_curve().get_x().to_string() << "\n";

  std::cout << "=============== commitment ===========\n";
  std::cout << "commitment: " << com() << "\n";

  return 0;
}
