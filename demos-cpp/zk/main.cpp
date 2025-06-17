#include <memory>
#include <vector>

#include <cbmpc/crypto/base.h>

#include "demo_nizk.h"

typedef std::unique_ptr<demo_nizk_t> bm_nizk_ptr;

int main(int argc, const char* argv[])
{
  std::cout << "================ ZK Demo ===============\n";
  std::vector<std::unique_ptr<demo_nizk_t>> nizks;

  nizks.push_back(bm_nizk_ptr(new demo_uc_dl_t(coinbase::crypto::curve_p256)));

  for (int i = 0; i < nizks.size(); i++)
  {
    std::cout << "---------------- " << nizks[i]->name << " ----------------\n";
    std::cout << "\n***** Setup *****\n";
    nizks[i]->setup();
    std::cout << "\n***** Prove *****\n";
    nizks[i]->prove();
    std::cout << "\n***** Verify *****\n";
    auto err = nizks[i]->verify();
  }
  return 0;
}
