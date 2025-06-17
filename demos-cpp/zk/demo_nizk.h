#include <cbmpc/zk/zk_ec.h>
#include <cbmpc/zk/zk_elgamal_com.h>
#include <cbmpc/zk/zk_paillier.h>
#include <cbmpc/zk/zk_pedersen.h>

struct demo_nizk_t
{
  std::string name;
  uint64_t aux = 0;
  buf_t sid = coinbase::crypto::gen_random(16);

  demo_nizk_t(std::string name) : name(name) {}
  virtual ~demo_nizk_t() = default;

  virtual void setup() = 0;
  virtual void prove() = 0;
  virtual error_t verify() = 0;
  virtual uint64_t proof_size() = 0;
};

struct demo_uc_dl_t : public demo_nizk_t
{
  ecurve_t curve;
  coinbase::zk::uc_dl_t zk;
  ecc_point_t G, Q;
  mod_t q;
  bn_t w;

  demo_uc_dl_t(ecurve_t c)
      : demo_nizk_t(std::string("ZK_UC_DL-") + c.get_name()), curve(c), G(c.generator()), q(c.order())
  {
  }

  void setup()
  {
    w = bn_t::rand(q);
    std::cout << "Prover's private input w, a random number from Z_q: " << w << "\n";
    Q = w * G;
    std::cout << "Common input: Q = w * G: \n";
    std::cout << "  Q.x = " << Q.get_x() << "\n";
    std::cout << "  Q.y = " << Q.get_y() << "\n";
    std::cout << "Prover proves that he knows w such that Q = w * G.\n";
  }

  void prove()
  {
    zk.prove(Q, w, sid, aux);
    std::cout << "Prover calls zk.prove(Q, w, sid, aux) to generate a proof.\n";
    std::cout << "Prover's proof contains : A[16], e[16], z[16], where 16 is "
                 "the Fischlin parameters we use.\n";
    std::cout << "  A[0].x = " << zk.A[0].get_x() << "\n";
    std::cout << "  A[0].y = " << zk.A[0].get_y() << "\n";
    std::cout << "  e[0] = " << zk.e[0] << "\n";
    std::cout << "  z[0] = " << zk.z[0] << "\n";
    std::cout << "  ...\n";
    std::cout << "The proof size is " << proof_size() << " bytes.\n";
  }
  error_t verify()
  {
    std::cout << "Verifier calls zk.verify(Q, sid, aux) to verify the proof.\n";
    error_t rv = zk.verify(Q, sid, aux);
    if (rv == 0) std::cout << "The proof is valid.\n";
    else std::cout << "The proof is invalid.\n";
    return rv;
  }
  uint64_t proof_size() { return coinbase::converter_t::convert_write(zk, 0); }
};
