#include <gtest/gtest-spi.h>
#include <gtest/gtest.h>

#include <cbmpc/core/log.h>
#include <cbmpc/crypto/base.h>
#include <cbmpc/crypto/ro.h>

#include "utils/test_macros.h"

using namespace coinbase;
using namespace coinbase::crypto;

namespace {

#ifdef _DEBUG
TEST(Mod, Constant) {
  EXPECT_EQ(bn_t(2).pow(2049) - 1449, LARGEST_PRIME_MOD_2048);
  EXPECT_TRUE(bn_t(LARGEST_PRIME_MOD_2048).prime());
}
#endif

// BN_MONT_CTX_set used to initialize mod_t only works with odd numbers
TEST(Mod, Initialization) {
  EXPECT_NO_FATAL_FAILURE(mod_t(3));
  EXPECT_NO_FATAL_FAILURE(mod_t(7));
  EXPECT_NO_FATAL_FAILURE(mod_t(11));
  EXPECT_NO_FATAL_FAILURE(mod_t(21));
  EXPECT_NO_FATAL_FAILURE(mod_t(99));

  EXPECT_NO_FATAL_FAILURE(mod_t(bn_t::generate_prime(128, false)));
  EXPECT_NO_FATAL_FAILURE(mod_t(bn_t::generate_prime(256, false)));
  EXPECT_NO_FATAL_FAILURE(mod_t(bn_t::generate_prime(512, false)));
  EXPECT_NO_FATAL_FAILURE(mod_t(bn_t::generate_prime(1024, false)));
  EXPECT_NO_FATAL_FAILURE(mod_t(bn_t::generate_prime(2048, false)));
  // Paillier/RSA Modulo
  EXPECT_NO_FATAL_FAILURE(mod_t(bn_t::generate_prime(2048, false) * bn_t::generate_prime(2048, false)));

  dylog_disable_scope_t no_log_err;
  EXPECT_CB_ASSERT(mod_t(100), "BN_MONT_CTX_set failed");
  EXPECT_CB_ASSERT(mod_t(bn_t::rand_bitlen(256) * 2), "BN_MONT_CTX_set failed");
}

TEST(Mod, Add) {
  mod_t q = crypto::curve_ed25519.order();
  bn_t a = 5;
  bn_t overflow_a = q + a;
  bn_t b = 8;
  bn_t overflow_b = q + b;
  EXPECT_EQ(q.add(a, b), 13);
  bn_t c;
  MODULO(q) { c = a + b; }
  EXPECT_EQ(c, 13);

#ifdef _DEBUG
  EXPECT_DEATH(q.add(overflow_a, b), "out of range for constant-time operations");
  EXPECT_DEATH(q.add(a, overflow_b), "out of range for constant-time operations");
#endif

  {
    crypto::vartime_scope_t vartime_scope;
    EXPECT_EQ(q.add(overflow_a, b), 13);
    EXPECT_EQ(q.add(a, overflow_b), 13);
    EXPECT_EQ(q.add(overflow_a, overflow_b), 13);

    bn_t c;
    MODULO(q) { c = overflow_a + b; }
    EXPECT_EQ(c, 13);
    MODULO(q) { c = a + overflow_b; }
    EXPECT_EQ(c, 13);
    MODULO(q) { c = overflow_a + overflow_b; }
    EXPECT_EQ(c, 13);
  }
}

TEST(Mod, NegativeValues) {
  bn_t a = 5;
  bn_t b = -8;
  mod_t m = bn_t(37);
  bn_t c;

  {
    crypto::vartime_scope_t vartime_scope;
    MODULO(m) c = b + a;
    EXPECT_EQ(c, 34);
  }

  {
    crypto::vartime_scope_t vartime_scope;
    MODULO(m) c = b + 5;
    EXPECT_EQ(c, 34);
  }
}

TEST(Mod, Mul) {
  bn_t a = bn_t::from_string(
      "1880918412945968332318334447791823180853896675983297173546586866443291748090530797966721820795610725941278341058"
      "6652384103866249484482713046481399149707205689254760988176900259143308909316831029572872662979713574607400194858"
      "559524831381751656699957722198731629445553297989202085151053236347020472027698118302788");
  bn_t b = bn_t::from_string(
      "2207956243932536463718181102629692112379729716009854443356856056893426409977162797028587368813231825439117757170"
      "1104155510584543753060010242551783179911557247430802018310376887558925216848420541855128731998730217440938501790"
      "03482685002468812744712868380915223194930588374249930584966352382234355913198190562652");
  mod_t m = mod_t(bn_t::from_string(
      "1898662926275647278849639489752123399541197588020875448768724478390869972129120711155087120991795732789780966917"
      "7092500935034799007921229207297261405836518950474145525801210638946547672612272831738654480554993891348979441979"
      "866690159979692710065051662974191361798813234762685123690214115935199910443053760928393"));
  bn_t c = bn_t::from_string(
      "6768961616439593786306577612520573247679878612879184168359557853575611421698469508048493477666835203152820758810"
      "0518991206576329667075450734141275016847256017037323753959523473214880944793130018528226081974772690788340297819"
      "03810422555237619343834454373315287214236553635191146766824291837543055084423392069");
  EXPECT_EQ(m.mul(a, b), c);
}

TEST(Mod, Inverse) {
  bn_t a = bn_t::from_string(
      "1279908216736355875139610681258425478766200788565806200053198682414864020760950418029694268793325515744532404474"
      "4445646499322542538445030211281515467694886387932620636779067850424216585522032952692012827721694540865160339347"
      "2455687115772883133332522509263658698320433569878161145025549750411311941107483980850");
  mod_t m = mod_t(bn_t::from_string(
      "1644916216602029093747474634526296258274093314054844079963820946762592559418358501511701655681376505867788245487"
      "9607053577425483469458194767333749435239254743302430809293218423208006653071085948631007831415958284606255529227"
      "014495565650847234726003940330443106836773098440029643532351746594462730252695156541161"));
  bn_t r = bn_t::from_string(
      "1488166222019000176085917532070950610758175891668205850960179338342120054923869361843270421345161459941749341632"
      "4757571138787000268801232874677751903532783436745446538495084866984033597564770984525230067899766556479444532770"
      "474050810368549447023606579671361631241975727065595823655993633521268846260448543684529");
  EXPECT_EQ(m.inv(a), r);
}

TEST(Mod, VTMod) {
  crypto::vartime_scope_t v;
  bn_t a = bn_t::from_string(
      "1861942856036593718511858615985192181514200973199650867467597373711639396039119593116012169656244407572291777376"
      "9888432730017469109442455216064375118641258725145366319820309670743376744884273040315128060218309892190972050466"
      "406712544991930790180040043635499384141218210472783444448036748435436614983257113553933");
  mod_t m1 = bn_t::from_string(
      "1861942856036593718511858615985192181514200973199650867467597373711639396039119593116012169656244407572291777376"
      "9888432730017469109442455216064375118641258725145366319820309670743376744884273040315128060218309892190972050466"
      "406712544991930790180040043635499384141218210472783444448036748435436614983257113553933");
  bn_t m2 = 103;
  bn_t m3 = 107;

  EXPECT_GE(mod_t::mod(a, m1), 0);
  EXPECT_GE(mod_t::mod(a, m2), 0);
  EXPECT_EQ(mod_t::mod(a, m3), 0);
}

TEST(Mod, InvNPhiN) {
  for (int i = 0; i < 5; i++) {
    bn_t p = bn_t::generate_prime(1024, false);
    bn_t q = bn_t::generate_prime(1024, false);
    bn_t N = p * q;
    bn_t phi_N = (p - 1) * (q - 1);
    bn_t N_inv = mod_t::N_inv_mod_phiN_2048(N, phi_N);

    {
      crypto::vartime_scope_t v;
      EXPECT_EQ(mod_t::mod(N_inv * N, phi_N), 1);
    }
  }
}

TEST(Mod, Coprime) {
  {
    // 128-bit odd number (2^128 - 173)
    mod_t prime_m = bn_t::from_string("340282366920938463463374607431768211283");
    auto test_coprime_prime = [](const mod_t& prime_m) {
      EXPECT_TRUE(mod_t::coprime(bn_t(5), prime_m));
      EXPECT_TRUE(mod_t::coprime(prime_m - 1, prime_m));
      EXPECT_TRUE(mod_t::coprime(bn_t(1), prime_m));
      EXPECT_FALSE(mod_t::coprime(bn_t(0), prime_m));
      for (int i = 0; i < 10; i++) {
        bn_t rnd = bn_t::rand(prime_m);
        EXPECT_TRUE(mod_t::coprime(rnd, prime_m));
      }
    };

    test_coprime_prime(prime_m);
    {
      crypto::vartime_scope_t v;
      test_coprime_prime(prime_m);
    }
  }
  {
    // 128-bit composite odd number (2^128 - 1)
    mod_t comp_m = {bn_t::from_string("340282366920938463463374607431768211455"), false};
    auto test_coprime_composite = [](const mod_t& comp_m) {
      EXPECT_TRUE(mod_t::coprime(comp_m - 1, comp_m));
      EXPECT_TRUE(mod_t::coprime(bn_t(14), comp_m));
      EXPECT_FALSE(mod_t::coprime(bn_t(9), comp_m));
      EXPECT_TRUE(mod_t::coprime(1, comp_m));
      EXPECT_FALSE(mod_t::coprime(0, comp_m));
    };
    test_coprime_composite(comp_m);
    {
      crypto::vartime_scope_t v;
      test_coprime_composite(comp_m);
    }
  }
}

TEST(Mod, SCRInverse) {
  {
    // Use a well-known prime modulus (order of Ed25519) and explicitly ask for the SCR algorithm.
    mod_t q = crypto::curve_ed25519.order();

    // Deterministic small operand
    bn_t a = 5;
    bn_t inv_a = q.inv(a, mod_t::inv_algo_e::SCR);
    EXPECT_EQ(q.mul(inv_a, a), 1);

    // Randomised operands – repeat a few times to increase coverage.
    for (int i = 0; i < 5; i++) {
      bn_t rnd = bn_t::rand(q);
      if (rnd == 0) rnd = 1;  // Ensure non-zero so that inverse exists.
      bn_t inv_rnd = q.inv(rnd, mod_t::inv_algo_e::SCR);
      EXPECT_EQ(q.mul(inv_rnd, rnd), 1);
    }
  }
  {
    // Test against 2^128 - 1 to make sure the implementation does not suffer from overflows.
    // Also note that 2^128 - 1 is not a prime number.
    mod_t comp_m = {bn_t::from_string("340282366920938463463374607431768211455"), false};
    bn_t a = 7;
    bn_t inv_a = comp_m.inv(a, mod_t::inv_algo_e::SCR);
    std::cout << "inv_a = " << inv_a << std::endl;
    EXPECT_EQ(comp_m.mul(inv_a, a), 1);
  }
}

}  // namespace
