window.BENCHMARK_DATA = {
  "lastUpdate": 1761580220077,
  "repoUrl": "https://github.com/coinbase/cb-mpc",
  "entries": {
    "Benchmark": [
      {
        "commit": {
          "author": {
            "email": "Arash-Afshar@users.noreply.github.com",
            "name": "Arash Afshar",
            "username": "Arash-Afshar"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "3e545471b7e44a4f1858c6729184d6d4cccd6f50",
          "message": "feat: first release (#3)",
          "timestamp": "2025-03-21T10:42:58-06:00",
          "tree_id": "a14267dcc1c4572d5df58c559399a4a0ec47d107",
          "url": "https://github.com/coinbase/cb-mpc/commit/3e545471b7e44a4f1858c6729184d6d4cccd6f50"
        },
        "date": 1742577984465,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "ZK/Batch-DL/Prover/3/1",
            "value": 1351.798817120242,
            "unit": "us/iter",
            "extra": "iterations: 514\ncpu: 1351.3902140077823 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/1",
            "value": 904.17071316843,
            "unit": "us/iter",
            "extra": "iterations: 767\ncpu: 904.0237170795307 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Prover",
            "value": 1721.8113292069618,
            "unit": "us/iter",
            "extra": "iterations: 404\ncpu: 1721.7313663366335 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Prover",
            "value": 1276.4132881038947,
            "unit": "us/iter",
            "extra": "iterations: 538\ncpu: 1276.3716449814128 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Prover",
            "value": 4041.232091952841,
            "unit": "us/iter",
            "extra": "iterations: 174\ncpu: 4040.8667701149443 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Prover",
            "value": 3672.857189744368,
            "unit": "us/iter",
            "extra": "iterations: 195\ncpu: 3672.5143897435883 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Prover",
            "value": 12358.525754383696,
            "unit": "us/iter",
            "extra": "iterations: 57\ncpu: 12357.72978947368 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Prover",
            "value": 11809.25924590618,
            "unit": "us/iter",
            "extra": "iterations: 61\ncpu: 11809.134737704908 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/1",
            "value": 1452.6645684648154,
            "unit": "us/iter",
            "extra": "iterations: 482\ncpu: 1452.6519170124466 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/1",
            "value": 4880.87698601442,
            "unit": "us/iter",
            "extra": "iterations: 143\ncpu: 4880.512629370631 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Verifier",
            "value": 2282.8688827355113,
            "unit": "us/iter",
            "extra": "iterations: 307\ncpu: 2282.4273257329014 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Verifier",
            "value": 11811.809067794611,
            "unit": "us/iter",
            "extra": "iterations: 59\ncpu: 11811.693491525422 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Verifier",
            "value": 5608.733857141286,
            "unit": "us/iter",
            "extra": "iterations: 126\ncpu: 5608.291047619043 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Verifier",
            "value": 38106.80155553806,
            "unit": "us/iter",
            "extra": "iterations: 18\ncpu: 38105.912444444475 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Verifier",
            "value": 28084.400679999817,
            "unit": "us/iter",
            "extra": "iterations: 25\ncpu: 28083.295200000008 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Verifier",
            "value": 213130.17533323847,
            "unit": "us/iter",
            "extra": "iterations: 3\ncpu: 213119.25733333346 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Verifier's Challenge (1st round)",
            "value": 0.5126282483751701,
            "unit": "us/iter",
            "extra": "iterations: 1377304\ncpu: 0.5125914402339636 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Prover Message (2nd round)",
            "value": 9762.433638886452,
            "unit": "us/iter",
            "extra": "iterations: 72\ncpu: 9762.131874999981 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Final Verification",
            "value": 9223.985973682446,
            "unit": "us/iter",
            "extra": "iterations: 76\ncpu: 9223.55776315791 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 1st Message (1st round)",
            "value": 33430.73452381196,
            "unit": "us/iter",
            "extra": "iterations: 21\ncpu: 33426.75400000011 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Verifier's Challenge (2nd round)",
            "value": 2.050901056225204,
            "unit": "us/iter",
            "extra": "iterations: 340931\ncpu: 2.0507104546081245 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 2nd Message (3rd round)",
            "value": 373.70964815757605,
            "unit": "us/iter",
            "extra": "iterations: 1981\ncpu: 373.70233266027077 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Final Verification (3rd round)",
            "value": 35248.98809998831,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35248.328600000175 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 1st Message (1st round)",
            "value": 35642.45800000663,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35637.28745000034 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Verifier's Challenge (2nd round)",
            "value": 0.6002795627313335,
            "unit": "us/iter",
            "extra": "iterations: 1177217\ncpu: 0.6002314237731746 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 2nd Message (3rd round)",
            "value": 705.8245748746446,
            "unit": "us/iter",
            "extra": "iterations: 995\ncpu: 705.7626964824316 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two Paillier Equal / Final Verification (3rd round)",
            "value": 73606.34533334733,
            "unit": "us/iter",
            "extra": "iterations: 9\ncpu: 73599.76177777917 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 1st Message (1st round)",
            "value": 132163.7981999629,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 132150.09179999697 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 1st Message (1st round)",
            "value": 130386.09149998592,
            "unit": "us/iter",
            "extra": "iterations: 4\ncpu: 130377.81899999602 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.5230987173813614,
            "unit": "us/iter",
            "extra": "iterations: 1347088\ncpu: 0.5230535458707839 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.527717019044836,
            "unit": "us/iter",
            "extra": "iterations: 1334740\ncpu: 0.5276733895739965 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 6.060306147473143,
            "unit": "us/iter",
            "extra": "iterations: 111737\ncpu: 6.060127558462999 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 5.372523337156813,
            "unit": "us/iter",
            "extra": "iterations: 122080\ncpu: 5.372185042595146 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Final Verification (3rd round)",
            "value": 5.348481562734766,
            "unit": "us/iter",
            "extra": "iterations: 111676\ncpu: 5.348367867760377 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Final Verification (3rd round)",
            "value": 6.129886841318316,
            "unit": "us/iter",
            "extra": "iterations: 123561\ncpu: 6.12932250467367 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 1st Message (1st round)",
            "value": 20441.156617647826,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20439.288794117474 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 1st Message (1st round)",
            "value": 20490.68788235619,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20489.12388235217 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.6572899441060124,
            "unit": "us/iter",
            "extra": "iterations: 1072386\ncpu: 0.6572515679988472 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.6601548420294191,
            "unit": "us/iter",
            "extra": "iterations: 1072732\ncpu: 0.6599951917161112 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 354.9144390243934,
            "unit": "us/iter",
            "extra": "iterations: 1968\ncpu: 354.8991305894329 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 353.22379716020595,
            "unit": "us/iter",
            "extra": "iterations: 1972\ncpu: 353.18531135903663 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Final Verification (3rd round)",
            "value": 43720.124999993,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43709.06487500292 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Final Verification (3rd round)",
            "value": 43824.69750001405,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43819.967000001016 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Prover",
            "value": 988.1421148457478,
            "unit": "us/iter",
            "extra": "iterations: 714\ncpu: 987.7668207282967 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Prover",
            "value": 659.0842318701107,
            "unit": "us/iter",
            "extra": "iterations: 1048\ncpu: 659.0112690840077 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Verifier",
            "value": 764.5322934778327,
            "unit": "us/iter",
            "extra": "iterations: 920\ncpu: 764.4767891304498 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Verifier",
            "value": 3499.230709999211,
            "unit": "us/iter",
            "extra": "iterations: 200\ncpu: 3498.895649999838 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Prover",
            "value": 77.58562093301178,
            "unit": "us/iter",
            "extra": "iterations: 9067\ncpu: 77.58297342009658 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Verifier",
            "value": 140.06899465132278,
            "unit": "us/iter",
            "extra": "iterations: 5048\ncpu: 140.06331834389587 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Prover/3",
            "value": 2787.4992929692867,
            "unit": "us/iter",
            "extra": "iterations: 256\ncpu: 2774.7881289061916 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Verify/3",
            "value": 1081.5589525995254,
            "unit": "us/iter",
            "extra": "iterations: 654\ncpu: 1081.0570733944735 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Prover",
            "value": 77.99618101913356,
            "unit": "us/iter",
            "extra": "iterations: 8988\ncpu: 77.98696806853651 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Verifier",
            "value": 145.3951466448871,
            "unit": "us/iter",
            "extra": "iterations: 4903\ncpu: 145.384728329597 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Prover/3",
            "value": 258.7005301645949,
            "unit": "us/iter",
            "extra": "iterations: 2735\ncpu: 258.6695970749474 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Verify/3",
            "value": 366.81592891234044,
            "unit": "us/iter",
            "extra": "iterations: 1885\ncpu: 366.788010610067 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Prover/3",
            "value": 4236.283221558268,
            "unit": "us/iter",
            "extra": "iterations: 167\ncpu: 4235.770502993987 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Verify/3",
            "value": 996.1362002967924,
            "unit": "us/iter",
            "extra": "iterations: 674\ncpu: 996.0286409495515 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Prover",
            "value": 23499.510166675467,
            "unit": "us/iter",
            "extra": "iterations: 30\ncpu: 23497.7549000007 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Verifier",
            "value": 22511.67154839007,
            "unit": "us/iter",
            "extra": "iterations: 31\ncpu: 22510.57435483972 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Prover",
            "value": 92696.3419999538,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 92688.45837499385 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Verifier",
            "value": 88063.24557144762,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 88054.87871428243 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Prover",
            "value": 90853.99050002251,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 90847.5287499968 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Verifier",
            "value": 183345.55850003654,
            "unit": "us/iter",
            "extra": "iterations: 4\ncpu: 183335.32849999302 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Prover",
            "value": 331696.0839999865,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 331683.7129999897 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Verifier",
            "value": 17485.402975000852,
            "unit": "us/iter",
            "extra": "iterations: 40\ncpu: 17485.254275000272 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Prover",
            "value": 48062.98046666294,
            "unit": "us/iter",
            "extra": "iterations: 15\ncpu: 48061.27326666759 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Verifier",
            "value": 100172.83457143223,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 100164.96528572004 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Prover",
            "value": 383100.7339999814,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 383069.5450000121 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Verifier",
            "value": 116906.38016663493,
            "unit": "us/iter",
            "extra": "iterations: 6\ncpu: 116904.79616666494 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Prover",
            "value": 388428.89799980185,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 388316.602499998 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Verifier",
            "value": 127211.41120000537,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 127199.0196000047 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Prover",
            "value": 297357.9649999465,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 297334.68899999594 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Verifier",
            "value": 298865.9284999358,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 298847.15150001284 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "Arash-Afshar@users.noreply.github.com",
            "name": "Arash Afshar",
            "username": "Arash-Afshar"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "dc7ce2156cfc61366a0f5f70761dacf22a35008b",
          "message": "chore: cleanup (#5)\n\n* fix: properly publish commit in benchmark page\n* feat: support go 1.24\n* chore: remove unused local variables",
          "timestamp": "2025-03-26T07:27:31-06:00",
          "tree_id": "ba26a2b6e0ce54b406c4d6ccc73b19506060313a",
          "url": "https://github.com/coinbase/cb-mpc/commit/dc7ce2156cfc61366a0f5f70761dacf22a35008b"
        },
        "date": 1742998247554,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "ZK/Batch-DL/Prover/3/1",
            "value": 1366.831910679836,
            "unit": "us/iter",
            "extra": "iterations: 515\ncpu: 1366.6243203883496 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/1",
            "value": 920.5036740835976,
            "unit": "us/iter",
            "extra": "iterations: 764\ncpu: 920.4007028795811 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Prover",
            "value": 1745.3602537690456,
            "unit": "us/iter",
            "extra": "iterations: 398\ncpu: 1745.142236180904 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Prover",
            "value": 1319.390558441606,
            "unit": "us/iter",
            "extra": "iterations: 539\ncpu: 1319.2824397031538 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Prover",
            "value": 4230.634053569594,
            "unit": "us/iter",
            "extra": "iterations: 168\ncpu: 4195.347136904763 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Prover",
            "value": 3764.013508107628,
            "unit": "us/iter",
            "extra": "iterations: 185\ncpu: 3724.5983135135134 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Prover",
            "value": 13113.980696427267,
            "unit": "us/iter",
            "extra": "iterations: 56\ncpu: 12935.983160714284 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Prover",
            "value": 12469.628105264475,
            "unit": "us/iter",
            "extra": "iterations: 57\ncpu: 12468.244298245607 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/1",
            "value": 1465.101891539897,
            "unit": "us/iter",
            "extra": "iterations: 461\ncpu: 1465.0543644251607 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/1",
            "value": 4924.355319149685,
            "unit": "us/iter",
            "extra": "iterations: 141\ncpu: 4923.862439716304 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Verifier",
            "value": 2247.1249739407367,
            "unit": "us/iter",
            "extra": "iterations: 307\ncpu: 2246.529895765475 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Verifier",
            "value": 11584.66066101675,
            "unit": "us/iter",
            "extra": "iterations: 59\ncpu: 11584.007118644078 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Verifier",
            "value": 5572.750094487437,
            "unit": "us/iter",
            "extra": "iterations: 127\ncpu: 5572.183236220473 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Verifier",
            "value": 38477.28122221067,
            "unit": "us/iter",
            "extra": "iterations: 18\ncpu: 38477.38050000002 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Verifier",
            "value": 28684.194519992165,
            "unit": "us/iter",
            "extra": "iterations: 25\ncpu: 28683.199799999955 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Verifier",
            "value": 214953.1696666903,
            "unit": "us/iter",
            "extra": "iterations: 3\ncpu: 214950.30133333337 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Verifier's Challenge (1st round)",
            "value": 0.5154145836531363,
            "unit": "us/iter",
            "extra": "iterations: 1368450\ncpu: 0.5153938375534355 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Prover Message (2nd round)",
            "value": 10028.572624997245,
            "unit": "us/iter",
            "extra": "iterations: 72\ncpu: 10019.60219444441 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Final Verification",
            "value": 9247.133202703677,
            "unit": "us/iter",
            "extra": "iterations: 74\ncpu: 9245.776121621597 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 1st Message (1st round)",
            "value": 35175.90747618929,
            "unit": "us/iter",
            "extra": "iterations: 21\ncpu: 35175.02952380963 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Verifier's Challenge (2nd round)",
            "value": 2.055867438279038,
            "unit": "us/iter",
            "extra": "iterations: 346201\ncpu: 2.0556462344129414 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 2nd Message (3rd round)",
            "value": 355.4923449845743,
            "unit": "us/iter",
            "extra": "iterations: 1974\ncpu: 355.472268996961 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Final Verification (3rd round)",
            "value": 35340.17305000816,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35337.112300000226 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 1st Message (1st round)",
            "value": 35768.440949982505,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35764.078149999536 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Verifier's Challenge (2nd round)",
            "value": 0.6004863817165049,
            "unit": "us/iter",
            "extra": "iterations: 1186126\ncpu: 0.6004456685040208 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 2nd Message (3rd round)",
            "value": 710.8907482378172,
            "unit": "us/iter",
            "extra": "iterations: 993\ncpu: 710.7995186304205 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two Paillier Equal / Final Verification (3rd round)",
            "value": 73904.17466664681,
            "unit": "us/iter",
            "extra": "iterations: 9\ncpu: 73890.00066666541 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 1st Message (1st round)",
            "value": 130097.1808000213,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 130078.2245999983 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 1st Message (1st round)",
            "value": 129991.18200004888,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129980.63800000069 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.5157930519149808,
            "unit": "us/iter",
            "extra": "iterations: 1354330\ncpu: 0.5157257913507101 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.5191804962932564,
            "unit": "us/iter",
            "extra": "iterations: 1361413\ncpu: 0.519131089537121 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 5.996800687252094,
            "unit": "us/iter",
            "extra": "iterations: 109421\ncpu: 5.996504190237716 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 6.0000628916176995,
            "unit": "us/iter",
            "extra": "iterations: 128952\ncpu: 5.999617066815639 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Final Verification (3rd round)",
            "value": 6.493526569721187,
            "unit": "us/iter",
            "extra": "iterations: 132858\ncpu: 6.492985676436637 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Final Verification (3rd round)",
            "value": 6.13474625387332,
            "unit": "us/iter",
            "extra": "iterations: 117121\ncpu: 6.1344457612212215 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 1st Message (1st round)",
            "value": 20689.835705886308,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20688.483235294003 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 1st Message (1st round)",
            "value": 20569.58458824203,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20569.241500000397 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.6432189360865818,
            "unit": "us/iter",
            "extra": "iterations: 1092232\ncpu: 0.6432002907807057 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.6457990667235312,
            "unit": "us/iter",
            "extra": "iterations: 1086495\ncpu: 0.6457560688268038 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 354.8970040921161,
            "unit": "us/iter",
            "extra": "iterations: 1955\ncpu: 354.88796470587675 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 354.9499267919314,
            "unit": "us/iter",
            "extra": "iterations: 1967\ncpu: 354.92348500255895 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Final Verification (3rd round)",
            "value": 43564.1607500088,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43560.29868750255 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Final Verification (3rd round)",
            "value": 43832.09118751097,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43830.89374999827 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Prover",
            "value": 1009.265947218429,
            "unit": "us/iter",
            "extra": "iterations: 701\ncpu: 1003.7355435093131 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Prover",
            "value": 679.6346179018079,
            "unit": "us/iter",
            "extra": "iterations: 1039\ncpu: 674.7451780558266 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Verifier",
            "value": 766.6025754611891,
            "unit": "us/iter",
            "extra": "iterations: 921\ncpu: 766.4196406080356 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Verifier",
            "value": 3526.409065328258,
            "unit": "us/iter",
            "extra": "iterations: 199\ncpu: 3526.205683417218 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Prover",
            "value": 77.86106322979637,
            "unit": "us/iter",
            "extra": "iterations: 9078\ncpu: 77.85860321656973 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Verifier",
            "value": 136.03550983504027,
            "unit": "us/iter",
            "extra": "iterations: 5033\ncpu: 136.02269421815777 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Prover/3",
            "value": 2773.5207768925766,
            "unit": "us/iter",
            "extra": "iterations: 251\ncpu: 2773.2071195218105 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Verify/3",
            "value": 1078.3302503960176,
            "unit": "us/iter",
            "extra": "iterations: 631\ncpu: 1078.0717290015739 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Prover",
            "value": 78.66593761140062,
            "unit": "us/iter",
            "extra": "iterations: 8976\ncpu: 78.65318415775552 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Verifier",
            "value": 143.72626152917542,
            "unit": "us/iter",
            "extra": "iterations: 4944\ncpu: 143.71401334950536 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Prover/3",
            "value": 256.8532315558863,
            "unit": "us/iter",
            "extra": "iterations: 2738\ncpu: 256.83275310445174 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Verify/3",
            "value": 370.3247947504585,
            "unit": "us/iter",
            "extra": "iterations: 1905\ncpu: 370.2935585301822 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Prover/3",
            "value": 4410.456417722747,
            "unit": "us/iter",
            "extra": "iterations: 158\ncpu: 4365.0794620252545 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Verify/3",
            "value": 1003.5313926027617,
            "unit": "us/iter",
            "extra": "iterations: 703\ncpu: 1003.4213826458497 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Prover",
            "value": 23551.80826666583,
            "unit": "us/iter",
            "extra": "iterations: 30\ncpu: 23551.109766664995 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Verifier",
            "value": 22640.522580654615,
            "unit": "us/iter",
            "extra": "iterations: 31\ncpu: 22638.157612902887 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Prover",
            "value": 92499.0817500202,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 92493.50300000003 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Verifier",
            "value": 88021.11987500894,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 88014.2793750025 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Prover",
            "value": 91429.50875002498,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 91420.7781250056 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Verifier",
            "value": 183933.01875005363,
            "unit": "us/iter",
            "extra": "iterations: 4\ncpu: 183921.128999998 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Prover",
            "value": 331735.25200004404,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 331731.3640000066 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Verifier",
            "value": 17564.879325004767,
            "unit": "us/iter",
            "extra": "iterations: 40\ncpu: 17564.104649999026 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Prover",
            "value": 48343.51792858017,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 48336.75985714438 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Verifier",
            "value": 99316.85142860911,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 99306.71442857277 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Prover",
            "value": 382602.2344999274,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 382568.2890000053 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Verifier",
            "value": 119406.29999996115,
            "unit": "us/iter",
            "extra": "iterations: 6\ncpu: 119396.30266667223 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Prover",
            "value": 389048.0649999972,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 389053.6064999992 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Verifier",
            "value": 127821.30839996171,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 127815.47719999935 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Prover",
            "value": 302194.8610000891,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 302113.4894999875 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Verifier",
            "value": 299593.9410000119,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 299553.692499984 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "d611643d608c17fadc7fa09793d00c84abfbaf94",
          "message": "feat: optimize Ed25519 point storage (#15)\n\n- Optimize Ed25519 point storage and operations for clarity and preparation for adding other curves.\n- Removed unused `set_coordinates` from `ecurve_interface_t`",
          "timestamp": "2025-05-05T22:41:02-07:00",
          "tree_id": "f6bbdb3391c7b9f77c66ab9d6afe21b91054bec3",
          "url": "https://github.com/coinbase/cb-mpc/commit/d611643d608c17fadc7fa09793d00c84abfbaf94"
        },
        "date": 1746512432007,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "ZK/Batch-DL/Prover/3/1",
            "value": 1360.339575875362,
            "unit": "us/iter",
            "extra": "iterations: 514\ncpu: 1360.2013151750973 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/1",
            "value": 1253.6032186379603,
            "unit": "us/iter",
            "extra": "iterations: 558\ncpu: 1253.4780448028675 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Prover",
            "value": 1755.0188400002753,
            "unit": "us/iter",
            "extra": "iterations: 400\ncpu: 1750.8536725000001 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Prover",
            "value": 1645.2938702829397,
            "unit": "us/iter",
            "extra": "iterations: 424\ncpu: 1638.618252358491 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Prover",
            "value": 4143.4580697658785,
            "unit": "us/iter",
            "extra": "iterations: 172\ncpu: 4123.8900058139525 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Prover",
            "value": 4214.979650888494,
            "unit": "us/iter",
            "extra": "iterations: 169\ncpu: 4188.135899408283 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Prover",
            "value": 12598.395109092631,
            "unit": "us/iter",
            "extra": "iterations: 55\ncpu: 12495.443309090911 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Prover",
            "value": 12723.345736838764,
            "unit": "us/iter",
            "extra": "iterations: 57\ncpu: 12607.48345614034 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/1",
            "value": 1448.8873347197537,
            "unit": "us/iter",
            "extra": "iterations: 481\ncpu: 1448.7060436590443 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/1",
            "value": 6181.8255486705475,
            "unit": "us/iter",
            "extra": "iterations: 113\ncpu: 6181.270929203546 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Verifier",
            "value": 2217.5748493585465,
            "unit": "us/iter",
            "extra": "iterations: 312\ncpu: 2217.431676282052 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Verifier",
            "value": 14666.755425530047,
            "unit": "us/iter",
            "extra": "iterations: 47\ncpu: 14664.909617021303 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Verifier",
            "value": 5699.0228809513255,
            "unit": "us/iter",
            "extra": "iterations: 126\ncpu: 5698.549063492067 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Verifier",
            "value": 48585.1328571698,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 48580.63957142861 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Verifier",
            "value": 27751.24792000497,
            "unit": "us/iter",
            "extra": "iterations: 25\ncpu: 27748.594040000044 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Verifier",
            "value": 272177.2590001213,
            "unit": "us/iter",
            "extra": "iterations: 3\ncpu: 272156.6890000003 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Verifier's Challenge (1st round)",
            "value": 0.5018562852657663,
            "unit": "us/iter",
            "extra": "iterations: 1397602\ncpu: 0.5017970674054535 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Prover Message (2nd round)",
            "value": 9731.878805559469,
            "unit": "us/iter",
            "extra": "iterations: 72\ncpu: 9730.998666666737 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Final Verification",
            "value": 9239.554306671684,
            "unit": "us/iter",
            "extra": "iterations: 75\ncpu: 9238.797946666657 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 1st Message (1st round)",
            "value": 33285.18323810602,
            "unit": "us/iter",
            "extra": "iterations: 21\ncpu: 33283.50042857138 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Verifier's Challenge (2nd round)",
            "value": 2.015440350977854,
            "unit": "us/iter",
            "extra": "iterations: 349310\ncpu: 2.0153229624116205 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 2nd Message (3rd round)",
            "value": 350.94092134259625,
            "unit": "us/iter",
            "extra": "iterations: 1996\ncpu: 350.9094188376728 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Final Verification (3rd round)",
            "value": 35109.79944999235,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35107.50350000009 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 1st Message (1st round)",
            "value": 35559.740300004705,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35555.86055000006 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Verifier's Challenge (2nd round)",
            "value": 0.5968849130828369,
            "unit": "us/iter",
            "extra": "iterations: 1174912\ncpu: 0.5968282628826724 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 2nd Message (3rd round)",
            "value": 704.3070603018652,
            "unit": "us/iter",
            "extra": "iterations: 995\ncpu: 704.238466331657 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two Paillier Equal / Final Verification (3rd round)",
            "value": 73489.12877776254,
            "unit": "us/iter",
            "extra": "iterations: 9\ncpu: 73484.19833333095 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 1st Message (1st round)",
            "value": 129492.77320003603,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129484.6963999987 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 1st Message (1st round)",
            "value": 129463.25779994368,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129451.89280000022 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.5145125610632261,
            "unit": "us/iter",
            "extra": "iterations: 1363340\ncpu: 0.5144809372570321 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.5166135488179332,
            "unit": "us/iter",
            "extra": "iterations: 1360591\ncpu: 0.5165841799629755 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 6.1515443300004335,
            "unit": "us/iter",
            "extra": "iterations: 100000\ncpu: 6.151471150000134 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 5.555195111802355,
            "unit": "us/iter",
            "extra": "iterations: 134610\ncpu: 5.5543310229553065 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Final Verification (3rd round)",
            "value": 6.358078969455137,
            "unit": "us/iter",
            "extra": "iterations: 119160\ncpu: 6.357500083920859 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Final Verification (3rd round)",
            "value": 6.003252055389186,
            "unit": "us/iter",
            "extra": "iterations: 110928\ncpu: 6.002619122313523 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 1st Message (1st round)",
            "value": 20467.8709705925,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20465.52355882374 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 1st Message (1st round)",
            "value": 20502.083647068717,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20500.317941177153 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.6432698246167758,
            "unit": "us/iter",
            "extra": "iterations: 1093262\ncpu: 0.643212864802771 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.6467182563202867,
            "unit": "us/iter",
            "extra": "iterations: 1086484\ncpu: 0.6466689725757584 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 352.4350583795307,
            "unit": "us/iter",
            "extra": "iterations: 1987\ncpu: 352.39971565174767 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 352.5451372353063,
            "unit": "us/iter",
            "extra": "iterations: 1982\ncpu: 352.51209132189666 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Final Verification (3rd round)",
            "value": 43734.41337500594,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43730.47874999969 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Final Verification (3rd round)",
            "value": 43854.51743752355,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43850.1416874999 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Prover",
            "value": 989.3400279722484,
            "unit": "us/iter",
            "extra": "iterations: 715\ncpu: 985.611634965048 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Prover",
            "value": 908.4404153642348,
            "unit": "us/iter",
            "extra": "iterations: 768\ncpu: 903.7248789062572 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Verifier",
            "value": 759.3769619149961,
            "unit": "us/iter",
            "extra": "iterations: 919\ncpu: 759.3278302502838 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Verifier",
            "value": 4511.7285192307645,
            "unit": "us/iter",
            "extra": "iterations: 156\ncpu: 4511.52166025628 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Prover",
            "value": 77.02836514571497,
            "unit": "us/iter",
            "extra": "iterations: 9095\ncpu: 77.02106003298938 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Verifier",
            "value": 139.01055921567104,
            "unit": "us/iter",
            "extra": "iterations: 5100\ncpu: 138.9811137254935 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Prover/3",
            "value": 2711.144171874125,
            "unit": "us/iter",
            "extra": "iterations: 256\ncpu: 2710.8099414061558 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Verify/3",
            "value": 1067.6474274813845,
            "unit": "us/iter",
            "extra": "iterations: 655\ncpu: 1067.5907358779307 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Prover",
            "value": 77.96163146265563,
            "unit": "us/iter",
            "extra": "iterations: 9052\ncpu: 77.95168338489093 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Verifier",
            "value": 139.90043772826954,
            "unit": "us/iter",
            "extra": "iterations: 4930\ncpu: 139.890153144022 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Prover/3",
            "value": 255.44622707420098,
            "unit": "us/iter",
            "extra": "iterations: 2748\ncpu: 255.41421542938812 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Verify/3",
            "value": 379.18942082890754,
            "unit": "us/iter",
            "extra": "iterations: 1882\ncpu: 379.1565456960627 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Prover/3",
            "value": 4259.377760478425,
            "unit": "us/iter",
            "extra": "iterations: 167\ncpu: 4232.687047904219 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Verify/3",
            "value": 994.0503517725978,
            "unit": "us/iter",
            "extra": "iterations: 705\ncpu: 993.9583787233802 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Prover",
            "value": 23571.159200006754,
            "unit": "us/iter",
            "extra": "iterations: 30\ncpu: 23568.247933333925 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Verifier",
            "value": 22532.81390322891,
            "unit": "us/iter",
            "extra": "iterations: 31\ncpu: 22531.662677419805 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Prover",
            "value": 92597.3517500438,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 92593.0323749995 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Verifier",
            "value": 87813.09912501456,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 87808.74837500363 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Prover",
            "value": 91055.84637501351,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 91051.64937500376 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Verifier",
            "value": 183736.92850002498,
            "unit": "us/iter",
            "extra": "iterations: 4\ncpu: 183717.76150000584 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Prover",
            "value": 333367.5994999794,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 333336.0759999948 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Verifier",
            "value": 17440.008224991743,
            "unit": "us/iter",
            "extra": "iterations: 40\ncpu: 17438.125025000772 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Prover",
            "value": 48348.71100001692,
            "unit": "us/iter",
            "extra": "iterations: 15\ncpu: 48342.45119999802 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Verifier",
            "value": 99092.43000005778,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 99085.281714289 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Prover",
            "value": 382128.7265000137,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 382114.85799999424 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Verifier",
            "value": 117257.76166667855,
            "unit": "us/iter",
            "extra": "iterations: 6\ncpu: 117247.99800000103 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Prover",
            "value": 386959.26300010795,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 386943.26300000626 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Verifier",
            "value": 127186.0148000087,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 127174.77480000525 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Prover",
            "value": 296870.2530001792,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 296848.75449999026 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Verifier",
            "value": 297511.87650003884,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 297489.6739999906 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "d96fee319fd9dc0820d8995be86a2f23ed1093f3",
          "message": "fix: RSA-OAEP MGF1 Misconfiguration (#16)\n\nPreviously, the RSA-OAEP implementation called the main hash setter twice instead of specifically setting the MGF1 hash. This caused MGF1 to incorrectly fall back to the default hash (SHA-1) instead of using the intended hash algorithm.",
          "timestamp": "2025-05-06T06:48:03-07:00",
          "tree_id": "d3ba2915cbb40e4c27ac2d51a315e7fb23cd2d17",
          "url": "https://github.com/coinbase/cb-mpc/commit/d96fee319fd9dc0820d8995be86a2f23ed1093f3"
        },
        "date": 1746541581191,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "ZK/Batch-DL/Prover/3/1",
            "value": 1368.1705976786513,
            "unit": "us/iter",
            "extra": "iterations: 517\ncpu: 1368.0545493230175 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/1",
            "value": 1242.6742141594807,
            "unit": "us/iter",
            "extra": "iterations: 565\ncpu: 1242.5191592920355 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Prover",
            "value": 1732.0236461915908,
            "unit": "us/iter",
            "extra": "iterations: 407\ncpu: 1731.8618181818183 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Prover",
            "value": 1621.3464462242773,
            "unit": "us/iter",
            "extra": "iterations: 437\ncpu: 1621.1023707093823 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Prover",
            "value": 4149.002708333325,
            "unit": "us/iter",
            "extra": "iterations: 168\ncpu: 4121.1951785714255 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Prover",
            "value": 4113.5420689659895,
            "unit": "us/iter",
            "extra": "iterations: 174\ncpu: 4084.689137931037 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Prover",
            "value": 13049.056410713838,
            "unit": "us/iter",
            "extra": "iterations: 56\ncpu: 12959.326928571429 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Prover",
            "value": 12780.484528303652,
            "unit": "us/iter",
            "extra": "iterations: 53\ncpu: 12779.337245283015 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/1",
            "value": 1462.9617145833624,
            "unit": "us/iter",
            "extra": "iterations: 480\ncpu: 1462.9002604166644 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/1",
            "value": 6175.49911504461,
            "unit": "us/iter",
            "extra": "iterations: 113\ncpu: 6174.717725663705 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Verifier",
            "value": 2205.3093343849573,
            "unit": "us/iter",
            "extra": "iterations: 317\ncpu: 2205.0111829653015 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Verifier",
            "value": 14730.707520830794,
            "unit": "us/iter",
            "extra": "iterations: 48\ncpu: 14730.01429166665 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Verifier",
            "value": 5575.642309523458,
            "unit": "us/iter",
            "extra": "iterations: 126\ncpu: 5574.853531746028 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Verifier",
            "value": 48591.08457141507,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 48585.365500000014 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Verifier",
            "value": 28240.302720005275,
            "unit": "us/iter",
            "extra": "iterations: 25\ncpu: 28238.31215999995 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Verifier",
            "value": 272230.23999999894,
            "unit": "us/iter",
            "extra": "iterations: 3\ncpu: 272196.63899999997 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Verifier's Challenge (1st round)",
            "value": 0.5115697255576376,
            "unit": "us/iter",
            "extra": "iterations: 1365059\ncpu: 0.5115283368704197 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Prover Message (2nd round)",
            "value": 9742.707277777325,
            "unit": "us/iter",
            "extra": "iterations: 72\ncpu: 9741.442874999982 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Final Verification",
            "value": 9240.411666669388,
            "unit": "us/iter",
            "extra": "iterations: 75\ncpu: 9239.541880000008 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 1st Message (1st round)",
            "value": 33449.605380957066,
            "unit": "us/iter",
            "extra": "iterations: 21\ncpu: 33444.42852380983 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Verifier's Challenge (2nd round)",
            "value": 2.0423017573021007,
            "unit": "us/iter",
            "extra": "iterations: 342855\ncpu: 2.0420807688381744 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 2nd Message (3rd round)",
            "value": 351.0896680060258,
            "unit": "us/iter",
            "extra": "iterations: 1991\ncpu: 351.07114364640586 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Final Verification (3rd round)",
            "value": 35199.146399997975,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35195.64360000018 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 1st Message (1st round)",
            "value": 35682.83900001461,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35678.721699999725 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Verifier's Challenge (2nd round)",
            "value": 0.6013722500852676,
            "unit": "us/iter",
            "extra": "iterations: 1167809\ncpu: 0.6013237498597711 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 2nd Message (3rd round)",
            "value": 703.7739366199603,
            "unit": "us/iter",
            "extra": "iterations: 994\ncpu: 703.7312887323958 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two Paillier Equal / Final Verification (3rd round)",
            "value": 73595.27777776343,
            "unit": "us/iter",
            "extra": "iterations: 9\ncpu: 73583.49766666569 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 1st Message (1st round)",
            "value": 129880.79719998497,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129877.31480000094 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 1st Message (1st round)",
            "value": 129615.47380000411,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129564.70859999741 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.5163876423888769,
            "unit": "us/iter",
            "extra": "iterations: 1358515\ncpu: 0.516356393562094 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.5185108853754177,
            "unit": "us/iter",
            "extra": "iterations: 1356453\ncpu: 0.5184725987557263 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 5.968978246890485,
            "unit": "us/iter",
            "extra": "iterations: 133728\ncpu: 5.968486285594504 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 6.675752760002069,
            "unit": "us/iter",
            "extra": "iterations: 100000\ncpu: 6.674792069999853 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Final Verification (3rd round)",
            "value": 5.604086889998143,
            "unit": "us/iter",
            "extra": "iterations: 100000\ncpu: 5.603544189999923 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Final Verification (3rd round)",
            "value": 5.381723330582916,
            "unit": "us/iter",
            "extra": "iterations: 113782\ncpu: 5.381162565256538 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 1st Message (1st round)",
            "value": 20441.05641176857,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20439.386235293332 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 1st Message (1st round)",
            "value": 20450.382794104586,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20446.569352941424 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.6490340707210105,
            "unit": "us/iter",
            "extra": "iterations: 1084685\ncpu: 0.6489601109999614 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.6464297134203787,
            "unit": "us/iter",
            "extra": "iterations: 1084446\ncpu: 0.6463795440252311 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 353.4335164555228,
            "unit": "us/iter",
            "extra": "iterations: 1975\ncpu: 353.410932151893 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 353.5448629721373,
            "unit": "us/iter",
            "extra": "iterations: 1985\ncpu: 353.52453148614524 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Final Verification (3rd round)",
            "value": 44048.09625000894,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 44046.32643750261 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Final Verification (3rd round)",
            "value": 43779.725749999445,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43775.28118750007 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Prover",
            "value": 981.3960410183798,
            "unit": "us/iter",
            "extra": "iterations: 707\ncpu: 981.3021909476923 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Prover",
            "value": 905.3275406451223,
            "unit": "us/iter",
            "extra": "iterations: 775\ncpu: 904.282882580637 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Verifier",
            "value": 763.2093333334096,
            "unit": "us/iter",
            "extra": "iterations: 921\ncpu: 763.1678783930765 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Verifier",
            "value": 4506.996632256144,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4506.76474838694 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Prover",
            "value": 77.11052729477856,
            "unit": "us/iter",
            "extra": "iterations: 9086\ncpu: 77.10856174334056 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Verifier",
            "value": 138.99903430596692,
            "unit": "us/iter",
            "extra": "iterations: 5072\ncpu: 138.97904830441357 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Prover/3",
            "value": 2710.4698022813923,
            "unit": "us/iter",
            "extra": "iterations: 263\ncpu: 2702.720144486778 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Verify/3",
            "value": 1072.7268338410051,
            "unit": "us/iter",
            "extra": "iterations: 656\ncpu: 1072.6165259146564 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Prover",
            "value": 77.52419379586021,
            "unit": "us/iter",
            "extra": "iterations: 8994\ncpu: 77.52113386702588 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Verifier",
            "value": 143.54206771037767,
            "unit": "us/iter",
            "extra": "iterations: 4918\ncpu: 143.51180540869822 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Prover/3",
            "value": 254.94698543341056,
            "unit": "us/iter",
            "extra": "iterations: 2746\ncpu: 254.93158521486194 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Verify/3",
            "value": 365.2588328925716,
            "unit": "us/iter",
            "extra": "iterations: 1891\ncpu: 365.2345489159403 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Prover/3",
            "value": 4166.985464285929,
            "unit": "us/iter",
            "extra": "iterations: 168\ncpu: 4166.832077380888 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Verify/3",
            "value": 994.8005241581773,
            "unit": "us/iter",
            "extra": "iterations: 683\ncpu: 994.7139297218179 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Prover",
            "value": 23505.282566657115,
            "unit": "us/iter",
            "extra": "iterations: 30\ncpu: 23503.28329999911 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Verifier",
            "value": 22563.408322580908,
            "unit": "us/iter",
            "extra": "iterations: 31\ncpu: 22561.781483870487 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Prover",
            "value": 92469.3682857521,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 92463.31071428975 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Verifier",
            "value": 91824.03774997283,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 91822.87287499946 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Prover",
            "value": 90753.84750002513,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 90750.83199999767 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Verifier",
            "value": 184005.2034999644,
            "unit": "us/iter",
            "extra": "iterations: 4\ncpu: 183992.22425000515 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Prover",
            "value": 332670.16950003384,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 332638.8824999924 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Verifier",
            "value": 17505.685025003004,
            "unit": "us/iter",
            "extra": "iterations: 40\ncpu: 17503.59655000011 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Prover",
            "value": 48197.74186668534,
            "unit": "us/iter",
            "extra": "iterations: 15\ncpu: 48195.43393333182 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Verifier",
            "value": 99053.61042852487,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 99044.97328571032 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Prover",
            "value": 383166.5555001109,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 383105.5029999959 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Verifier",
            "value": 116959.36700001161,
            "unit": "us/iter",
            "extra": "iterations: 6\ncpu: 116945.87700000625 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Prover",
            "value": 392024.52950007684,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 391988.70849998004 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Verifier",
            "value": 127029.63359997739,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 127015.33240000345 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Prover",
            "value": 297349.37599982915,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 297329.47799999465 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Verifier",
            "value": 298553.906999814,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 298521.37800000375 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "225d67963edc510a9d3f83ce87fa780dbf256a25",
          "message": "fix: Zeroize plaintext on AES-GCM decryption failure (#17)\n\nPreviously, the AES-GCM `decrypt_final` function did not zeroize the decrypted plaintext from memory when the authentication tag verification failed. This could lead to sensitive data remaining in memory. We emphasize that none of the higher level protocols are impacted by this as they already zeroize the value. We are making this change for good hygiene.\n\nThis commit leverages `EVP_CIPHER_CTX` to ensure proper cleanup and zeroization of the plaintext buffer in the event of a decryption failure.",
          "timestamp": "2025-05-06T07:36:35-07:00",
          "tree_id": "597d1a2940de34c5960ba368686aa44c64bf65d7",
          "url": "https://github.com/coinbase/cb-mpc/commit/225d67963edc510a9d3f83ce87fa780dbf256a25"
        },
        "date": 1746544411485,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "ZK/Batch-DL/Prover/3/1",
            "value": 1346.1415145072249,
            "unit": "us/iter",
            "extra": "iterations: 517\ncpu: 1346.1143675048356 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/1",
            "value": 1245.542287744674,
            "unit": "us/iter",
            "extra": "iterations: 563\ncpu: 1245.236191829485 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Prover",
            "value": 1723.4769126206215,
            "unit": "us/iter",
            "extra": "iterations: 412\ncpu: 1718.7469441747573 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Prover",
            "value": 1621.6875678158735,
            "unit": "us/iter",
            "extra": "iterations: 435\ncpu: 1613.8993655172414 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Prover",
            "value": 4162.457343194998,
            "unit": "us/iter",
            "extra": "iterations: 169\ncpu: 4140.290153846152 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Prover",
            "value": 4038.5153333328612,
            "unit": "us/iter",
            "extra": "iterations: 174\ncpu: 4037.993011494258 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Prover",
            "value": 12501.355094344653,
            "unit": "us/iter",
            "extra": "iterations: 53\ncpu: 12453.5540754717 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Prover",
            "value": 13358.79696226876,
            "unit": "us/iter",
            "extra": "iterations: 53\ncpu: 13197.522584905668 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/1",
            "value": 1471.9251854160123,
            "unit": "us/iter",
            "extra": "iterations: 480\ncpu: 1457.790122916667 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/1",
            "value": 6218.251831861196,
            "unit": "us/iter",
            "extra": "iterations: 113\ncpu: 6217.448592920347 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Verifier",
            "value": 2248.8537225810082,
            "unit": "us/iter",
            "extra": "iterations: 310\ncpu: 2248.6206774193565 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Verifier",
            "value": 14726.049312505058,
            "unit": "us/iter",
            "extra": "iterations: 48\ncpu: 14725.767499999987 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Verifier",
            "value": 5412.460064514064,
            "unit": "us/iter",
            "extra": "iterations: 124\ncpu: 5411.762959677427 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Verifier",
            "value": 48816.62385715962,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 48814.42471428571 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Verifier",
            "value": 28526.937333329744,
            "unit": "us/iter",
            "extra": "iterations: 24\ncpu: 28525.379125000047 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Verifier",
            "value": 273661.5543332543,
            "unit": "us/iter",
            "extra": "iterations: 3\ncpu: 273642.9633333337 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Verifier's Challenge (1st round)",
            "value": 0.5114356931789882,
            "unit": "us/iter",
            "extra": "iterations: 1379737\ncpu: 0.5114172519835309 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Prover Message (2nd round)",
            "value": 9803.863402775429,
            "unit": "us/iter",
            "extra": "iterations: 72\ncpu: 9803.162375000045 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Final Verification",
            "value": 9344.629079999626,
            "unit": "us/iter",
            "extra": "iterations: 75\ncpu: 9344.210493333283 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 1st Message (1st round)",
            "value": 33295.16585712814,
            "unit": "us/iter",
            "extra": "iterations: 21\ncpu: 33291.931666666875 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Verifier's Challenge (2nd round)",
            "value": 2.027202780215989,
            "unit": "us/iter",
            "extra": "iterations: 347455\ncpu: 2.027185120375321 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 2nd Message (3rd round)",
            "value": 351.8781199197209,
            "unit": "us/iter",
            "extra": "iterations: 1993\ncpu: 351.860729553435 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Final Verification (3rd round)",
            "value": 37139.92005264357,
            "unit": "us/iter",
            "extra": "iterations: 19\ncpu: 37139.53426315778 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 1st Message (1st round)",
            "value": 35772.14874999299,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35770.196100000365 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Verifier's Challenge (2nd round)",
            "value": 0.5999613180198663,
            "unit": "us/iter",
            "extra": "iterations: 1170028\ncpu: 0.5998787422181336 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 2nd Message (3rd round)",
            "value": 704.796770576279,
            "unit": "us/iter",
            "extra": "iterations: 972\ncpu: 704.7693631687386 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two Paillier Equal / Final Verification (3rd round)",
            "value": 73496.20722223133,
            "unit": "us/iter",
            "extra": "iterations: 9\ncpu: 73493.16500000213 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 1st Message (1st round)",
            "value": 129631.92639999762,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129613.52799999873 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 1st Message (1st round)",
            "value": 130147.67900003791,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 130138.8309999993 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.5234482416340979,
            "unit": "us/iter",
            "extra": "iterations: 1340904\ncpu: 0.523410275455965 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.5222346683769363,
            "unit": "us/iter",
            "extra": "iterations: 1320506\ncpu: 0.5221403484724803 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 7.01813132775153,
            "unit": "us/iter",
            "extra": "iterations: 139544\ncpu: 7.017283888952773 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 7.281629874655443,
            "unit": "us/iter",
            "extra": "iterations: 103238\ncpu: 7.280710300470742 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Final Verification (3rd round)",
            "value": 6.347362131346268,
            "unit": "us/iter",
            "extra": "iterations: 138748\ncpu: 6.346931191800976 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Final Verification (3rd round)",
            "value": 5.7162646564626005,
            "unit": "us/iter",
            "extra": "iterations: 130335\ncpu: 5.715868208846449 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 1st Message (1st round)",
            "value": 20484.896735297298,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20481.077911764554 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 1st Message (1st round)",
            "value": 20472.03541177195,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20471.25799999977 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.6447837372770294,
            "unit": "us/iter",
            "extra": "iterations: 1088477\ncpu: 0.6447584937486053 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.6438695490886822,
            "unit": "us/iter",
            "extra": "iterations: 1091744\ncpu: 0.6437991186578617 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 353.00606555726813,
            "unit": "us/iter",
            "extra": "iterations: 1983\ncpu: 352.9501664145164 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 352.56117397896924,
            "unit": "us/iter",
            "extra": "iterations: 1983\ncpu: 352.55723550174247 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Final Verification (3rd round)",
            "value": 44104.98174999589,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 44101.25331250114 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Final Verification (3rd round)",
            "value": 44248.76462499583,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 44243.09768750234 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Prover",
            "value": 987.3661568627878,
            "unit": "us/iter",
            "extra": "iterations: 714\ncpu: 984.9337324930261 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Prover",
            "value": 906.9483011587195,
            "unit": "us/iter",
            "extra": "iterations: 777\ncpu: 906.7539601029588 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Verifier",
            "value": 758.7907830803446,
            "unit": "us/iter",
            "extra": "iterations: 922\ncpu: 758.691137744024 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Verifier",
            "value": 4482.676339744802,
            "unit": "us/iter",
            "extra": "iterations: 156\ncpu: 4482.156166666767 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Prover",
            "value": 77.61440468855791,
            "unit": "us/iter",
            "extra": "iterations: 9086\ncpu: 77.60708419547012 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Verifier",
            "value": 137.3999153376704,
            "unit": "us/iter",
            "extra": "iterations: 5079\ncpu: 137.39172711163283 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Prover/3",
            "value": 2745.451685822797,
            "unit": "us/iter",
            "extra": "iterations: 261\ncpu: 2718.443356321853 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Verify/3",
            "value": 1068.7151112803094,
            "unit": "us/iter",
            "extra": "iterations: 656\ncpu: 1068.6998567072899 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Prover",
            "value": 78.14112335140696,
            "unit": "us/iter",
            "extra": "iterations: 9023\ncpu: 78.13157076360562 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Verifier",
            "value": 143.24725570031217,
            "unit": "us/iter",
            "extra": "iterations: 4912\ncpu: 143.24178236970258 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Prover/3",
            "value": 254.8267989804006,
            "unit": "us/iter",
            "extra": "iterations: 2746\ncpu: 254.80082520028307 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Verify/3",
            "value": 371.8760550025662,
            "unit": "us/iter",
            "extra": "iterations: 1909\ncpu: 371.85314562597296 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Prover/3",
            "value": 4244.274113770989,
            "unit": "us/iter",
            "extra": "iterations: 167\ncpu: 4226.780784431222 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Verify/3",
            "value": 1001.554684658598,
            "unit": "us/iter",
            "extra": "iterations: 704\ncpu: 994.3295767045739 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Prover",
            "value": 23458.75150000817,
            "unit": "us/iter",
            "extra": "iterations: 30\ncpu: 23456.74583333448 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Verifier",
            "value": 22571.514645161158,
            "unit": "us/iter",
            "extra": "iterations: 31\ncpu: 22568.958064517396 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Prover",
            "value": 92457.34000000994,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 92448.55062499369 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Verifier",
            "value": 87885.05499995836,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 87882.22712500015 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Prover",
            "value": 90695.86850000633,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 90685.50874999915 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Verifier",
            "value": 183916.53924993533,
            "unit": "us/iter",
            "extra": "iterations: 4\ncpu: 183896.78100000139 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Prover",
            "value": 331690.54049994884,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 331673.8520000229 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Verifier",
            "value": 17555.52129999387,
            "unit": "us/iter",
            "extra": "iterations: 40\ncpu: 17553.765899999973 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Prover",
            "value": 48165.43553330727,
            "unit": "us/iter",
            "extra": "iterations: 15\ncpu: 48163.340399999775 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Verifier",
            "value": 99125.64985714717,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 99118.01985714419 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Prover",
            "value": 382017.18200002686,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 381997.8944999889 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Verifier",
            "value": 117478.07566666779,
            "unit": "us/iter",
            "extra": "iterations: 6\ncpu: 117466.28483333172 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Prover",
            "value": 386688.9920000176,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 386639.42750000046 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Verifier",
            "value": 127318.69680001182,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 127281.61899999579 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Prover",
            "value": 296928.29399982654,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 296895.61750001303 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Verifier",
            "value": 297906.5899999114,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 297847.2709999949 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "4360c2097e506d7e51bd7a19c9a74b6344f254aa",
          "message": "chore: Address ASan and UBSan findings (#18)",
          "timestamp": "2025-05-07T07:11:09-06:00",
          "tree_id": "6d9e4e2097e973523bb3492d6467dbdd02cff8de",
          "url": "https://github.com/coinbase/cb-mpc/commit/4360c2097e506d7e51bd7a19c9a74b6344f254aa"
        },
        "date": 1746625698706,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "ZK/Batch-DL/Prover/3/1",
            "value": 1347.425076775552,
            "unit": "us/iter",
            "extra": "iterations: 521\ncpu: 1347.3344510556622 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/1",
            "value": 1237.8293392541816,
            "unit": "us/iter",
            "extra": "iterations: 563\ncpu: 1237.6365985790412 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Prover",
            "value": 1713.8909605911053,
            "unit": "us/iter",
            "extra": "iterations: 406\ncpu: 1713.6462290640393 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Prover",
            "value": 1616.8761412037231,
            "unit": "us/iter",
            "extra": "iterations: 432\ncpu: 1610.7336087962963 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Prover",
            "value": 4116.1500930234815,
            "unit": "us/iter",
            "extra": "iterations: 172\ncpu: 4096.221005813953 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Prover",
            "value": 4030.3400689648615,
            "unit": "us/iter",
            "extra": "iterations: 174\ncpu: 4003.4680862068976 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Prover",
            "value": 12956.692785711053,
            "unit": "us/iter",
            "extra": "iterations: 56\ncpu: 12854.17807142857 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Prover",
            "value": 12717.426929827381,
            "unit": "us/iter",
            "extra": "iterations: 57\ncpu: 12608.051631578937 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/1",
            "value": 1463.0776493776243,
            "unit": "us/iter",
            "extra": "iterations: 482\ncpu: 1462.9315373443967 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/1",
            "value": 6229.3770178558,
            "unit": "us/iter",
            "extra": "iterations: 112\ncpu: 6229.215428571435 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Verifier",
            "value": 2220.822871382476,
            "unit": "us/iter",
            "extra": "iterations: 311\ncpu: 2220.454733118971 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Verifier",
            "value": 15379.036127661551,
            "unit": "us/iter",
            "extra": "iterations: 47\ncpu: 15377.02546808509 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Verifier",
            "value": 5615.24375396922,
            "unit": "us/iter",
            "extra": "iterations: 126\ncpu: 5614.837912698408 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Verifier",
            "value": 52988.98378571591,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 52982.79942857142 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Verifier",
            "value": 27911.082119999264,
            "unit": "us/iter",
            "extra": "iterations: 25\ncpu: 27905.957719999962 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Verifier",
            "value": 280539.26600000524,
            "unit": "us/iter",
            "extra": "iterations: 3\ncpu: 280519.5489999998 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Verifier's Challenge (1st round)",
            "value": 0.5037874221346814,
            "unit": "us/iter",
            "extra": "iterations: 1400174\ncpu: 0.5037401465817818 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Prover Message (2nd round)",
            "value": 9742.989577460603,
            "unit": "us/iter",
            "extra": "iterations: 71\ncpu: 9741.359323943594 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Final Verification",
            "value": 9278.389866667567,
            "unit": "us/iter",
            "extra": "iterations: 75\ncpu: 9277.072186666639 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 1st Message (1st round)",
            "value": 33331.158666658754,
            "unit": "us/iter",
            "extra": "iterations: 21\ncpu: 33328.995047619224 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Verifier's Challenge (2nd round)",
            "value": 2.002912098182229,
            "unit": "us/iter",
            "extra": "iterations: 349879\ncpu: 2.002607967325854 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 2nd Message (3rd round)",
            "value": 362.94769111002523,
            "unit": "us/iter",
            "extra": "iterations: 1991\ncpu: 362.90765042691993 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Final Verification (3rd round)",
            "value": 35179.85045000387,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35178.72729999993 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 1st Message (1st round)",
            "value": 35604.69189999367,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35601.61329999971 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Verifier's Challenge (2nd round)",
            "value": 0.5990033369430138,
            "unit": "us/iter",
            "extra": "iterations: 1179223\ncpu: 0.5989235098026293 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 2nd Message (3rd round)",
            "value": 710.5843380282435,
            "unit": "us/iter",
            "extra": "iterations: 994\ncpu: 710.5285774647887 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two Paillier Equal / Final Verification (3rd round)",
            "value": 76693.58666665478,
            "unit": "us/iter",
            "extra": "iterations: 9\ncpu: 76685.20355555744 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 1st Message (1st round)",
            "value": 129649.34040001026,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129635.3350000004 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 1st Message (1st round)",
            "value": 129596.4811999511,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129589.45940000036 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.5138209888314055,
            "unit": "us/iter",
            "extra": "iterations: 1368747\ncpu: 0.5137729711918975 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.511440526962585,
            "unit": "us/iter",
            "extra": "iterations: 1370268\ncpu: 0.511404535463126 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 5.132110915450343,
            "unit": "us/iter",
            "extra": "iterations: 128909\ncpu: 5.131521290212537 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 6.008535220293147,
            "unit": "us/iter",
            "extra": "iterations: 146691\ncpu: 6.007997089119323 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Final Verification (3rd round)",
            "value": 4.470611179565956,
            "unit": "us/iter",
            "extra": "iterations: 120112\ncpu: 4.470052792393895 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Final Verification (3rd round)",
            "value": 6.065472150003188,
            "unit": "us/iter",
            "extra": "iterations: 100000\ncpu: 6.064879779999899 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 1st Message (1st round)",
            "value": 20503.106441176842,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20500.444794117644 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 1st Message (1st round)",
            "value": 20456.254970587695,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20453.368794118007 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.634200047679648,
            "unit": "us/iter",
            "extra": "iterations: 1111590\ncpu: 0.6328945555465499 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.6299385906776416,
            "unit": "us/iter",
            "extra": "iterations: 1110548\ncpu: 0.6298524152040303 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 366.03981424940065,
            "unit": "us/iter",
            "extra": "iterations: 1965\ncpu: 366.0242035623318 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 354.96305398587094,
            "unit": "us/iter",
            "extra": "iterations: 1982\ncpu: 354.91109132191565 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Final Verification (3rd round)",
            "value": 44101.72237498955,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 44095.927187498775 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Final Verification (3rd round)",
            "value": 43776.776937477276,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43774.11468750126 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Prover",
            "value": 988.3375056018218,
            "unit": "us/iter",
            "extra": "iterations: 714\ncpu: 984.5848165265999 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Prover",
            "value": 912.3773139530978,
            "unit": "us/iter",
            "extra": "iterations: 774\ncpu: 905.9869354005776 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Verifier",
            "value": 759.6092266813328,
            "unit": "us/iter",
            "extra": "iterations: 922\ncpu: 759.5872971800405 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Verifier",
            "value": 4524.919680271418,
            "unit": "us/iter",
            "extra": "iterations: 147\ncpu: 4524.528136054566 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Prover",
            "value": 77.25730439101454,
            "unit": "us/iter",
            "extra": "iterations: 9064\ncpu: 77.25312985437075 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Verifier",
            "value": 138.63232796984286,
            "unit": "us/iter",
            "extra": "iterations: 5034\ncpu: 138.61854568931363 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Prover/3",
            "value": 2751.6532702704994,
            "unit": "us/iter",
            "extra": "iterations: 259\ncpu: 2736.850521235513 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Verify/3",
            "value": 1069.8993914373023,
            "unit": "us/iter",
            "extra": "iterations: 654\ncpu: 1069.8488088684544 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Prover",
            "value": 78.13138306854974,
            "unit": "us/iter",
            "extra": "iterations: 9001\ncpu: 78.12787490278863 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Verifier",
            "value": 143.51575851522065,
            "unit": "us/iter",
            "extra": "iterations: 4903\ncpu: 143.51280930042176 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Prover/3",
            "value": 256.32305361045223,
            "unit": "us/iter",
            "extra": "iterations: 2742\ncpu: 256.2855590809579 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Verify/3",
            "value": 367.1196035939343,
            "unit": "us/iter",
            "extra": "iterations: 1892\ncpu: 367.11501532770126 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Prover/3",
            "value": 4248.291107783761,
            "unit": "us/iter",
            "extra": "iterations: 167\ncpu: 4226.038311377399 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Verify/3",
            "value": 999.7728309662854,
            "unit": "us/iter",
            "extra": "iterations: 704\ncpu: 999.7504559659279 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Prover",
            "value": 23435.54146667278,
            "unit": "us/iter",
            "extra": "iterations: 30\ncpu: 23431.71879999962 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Verifier",
            "value": 22552.42848386549,
            "unit": "us/iter",
            "extra": "iterations: 31\ncpu: 22551.75893548305 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Prover",
            "value": 92536.56949999822,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 92531.67625000457 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Verifier",
            "value": 87976.89237496798,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 87971.83099999728 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Prover",
            "value": 90723.88712496604,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 90713.25549999898 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Verifier",
            "value": 184196.18349992106,
            "unit": "us/iter",
            "extra": "iterations: 4\ncpu: 184190.74349999675 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Prover",
            "value": 333517.3964999285,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 333332.981500007 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Verifier",
            "value": 17448.108799999318,
            "unit": "us/iter",
            "extra": "iterations: 40\ncpu: 17444.98584999974 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Prover",
            "value": 48132.523200001735,
            "unit": "us/iter",
            "extra": "iterations: 15\ncpu: 48130.284666668405 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Verifier",
            "value": 99014.4870000092,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 99001.86957142815 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Prover",
            "value": 382314.4534999301,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 382279.2005000224 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Verifier",
            "value": 118587.20850000282,
            "unit": "us/iter",
            "extra": "iterations: 6\ncpu: 118581.39033333259 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Prover",
            "value": 387726.9019999403,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 387702.4370000015 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Verifier",
            "value": 127134.25680003638,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 127113.34360000138 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Prover",
            "value": 296572.355500075,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 296560.25049999356 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Verifier",
            "value": 297942.4914999527,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 297928.00850000844 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "032abd771b188319cd4ee8c3e7663e8602132b97",
          "message": "chore: Add usage notes to core functions (#20)\n\nThis PR adds informational comments and/or updates documentation to clarify that many core functions intentionally omit boundary and overflow checks. This design was a deliberate choice to maximize performance in critical code paths.\n\nDevelopers using these functions must be aware that the responsibility for input validation and preventing overflow or out-of-bounds errors lies with the calling code. While this approach benefits performance, misusing these functions without proper external checks could potentially lead to security vulnerabilities if input is not carefully managed by the caller.\n\nThese changes are for documentation and awareness purposes only and do not alter the functional behavior of the core functions. The goal is to prevent accidental misuse and ensure developers understand the intentional performance-security trade-off in these specific areas.",
          "timestamp": "2025-05-07T06:55:07-07:00",
          "tree_id": "49ea819c770c958ba2d2caaf61af1ed6d6303803",
          "url": "https://github.com/coinbase/cb-mpc/commit/032abd771b188319cd4ee8c3e7663e8602132b97"
        },
        "date": 1746628456039,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "ZK/Batch-DL/Prover/3/1",
            "value": 1346.266899807878,
            "unit": "us/iter",
            "extra": "iterations: 519\ncpu: 1346.132304431599 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/1",
            "value": 1236.2652180851412,
            "unit": "us/iter",
            "extra": "iterations: 564\ncpu: 1236.0657304964539 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Prover",
            "value": 1715.0793446600217,
            "unit": "us/iter",
            "extra": "iterations: 412\ncpu: 1714.9338179611655 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Prover",
            "value": 1607.5092668212258,
            "unit": "us/iter",
            "extra": "iterations: 431\ncpu: 1607.3694640371232 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Prover",
            "value": 4023.201385966227,
            "unit": "us/iter",
            "extra": "iterations: 171\ncpu: 4023.0549707602327 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Prover",
            "value": 3981.1231896556433,
            "unit": "us/iter",
            "extra": "iterations: 174\ncpu: 3980.6099942528767 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Prover",
            "value": 12336.8145932237,
            "unit": "us/iter",
            "extra": "iterations: 59\ncpu: 12336.323745762702 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Prover",
            "value": 12618.064406778705,
            "unit": "us/iter",
            "extra": "iterations: 59\ncpu: 12543.428559322027 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/1",
            "value": 1471.4217666664808,
            "unit": "us/iter",
            "extra": "iterations: 480\ncpu: 1471.244704166667 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/1",
            "value": 6261.196160713618,
            "unit": "us/iter",
            "extra": "iterations: 112\ncpu: 6260.631741071419 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Verifier",
            "value": 2224.786088050483,
            "unit": "us/iter",
            "extra": "iterations: 318\ncpu: 2224.5659779874213 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Verifier",
            "value": 14768.257319140483,
            "unit": "us/iter",
            "extra": "iterations: 47\ncpu: 14766.778340425515 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Verifier",
            "value": 5457.383395163296,
            "unit": "us/iter",
            "extra": "iterations: 124\ncpu: 5456.768096774197 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Verifier",
            "value": 48939.39171428688,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 48935.580285714255 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Verifier",
            "value": 27838.365880015772,
            "unit": "us/iter",
            "extra": "iterations: 25\ncpu: 27835.41692 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Verifier",
            "value": 274982.8843332883,
            "unit": "us/iter",
            "extra": "iterations: 3\ncpu: 274957.30066666665 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Verifier's Challenge (1st round)",
            "value": 0.49988637862852364,
            "unit": "us/iter",
            "extra": "iterations: 1400740\ncpu: 0.4998553428901861 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Prover Message (2nd round)",
            "value": 9766.552791669861,
            "unit": "us/iter",
            "extra": "iterations: 72\ncpu: 9765.891527777725 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Final Verification",
            "value": 9249.099799999385,
            "unit": "us/iter",
            "extra": "iterations: 75\ncpu: 9247.817040000067 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 1st Message (1st round)",
            "value": 33292.74852380661,
            "unit": "us/iter",
            "extra": "iterations: 21\ncpu: 33290.71733333305 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Verifier's Challenge (2nd round)",
            "value": 2.005531054386577,
            "unit": "us/iter",
            "extra": "iterations: 350659\ncpu: 2.005270427965638 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 2nd Message (3rd round)",
            "value": 354.98773165830306,
            "unit": "us/iter",
            "extra": "iterations: 1990\ncpu: 354.9676788944726 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Final Verification (3rd round)",
            "value": 35257.93655001053,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35256.54265 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 1st Message (1st round)",
            "value": 35613.89075000534,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35611.45359999997 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Verifier's Challenge (2nd round)",
            "value": 0.5972840880126401,
            "unit": "us/iter",
            "extra": "iterations: 1177012\ncpu: 0.5972215737817446 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 2nd Message (3rd round)",
            "value": 706.4229939641807,
            "unit": "us/iter",
            "extra": "iterations: 994\ncpu: 706.359067404414 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two Paillier Equal / Final Verification (3rd round)",
            "value": 73615.31199999912,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 73613.39400000182 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 1st Message (1st round)",
            "value": 130680.92820003585,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 130667.35759999801 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 1st Message (1st round)",
            "value": 129624.66140006654,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129603.29619999698 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.5125950190997229,
            "unit": "us/iter",
            "extra": "iterations: 1368588\ncpu: 0.5125646052720148 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.5129198838137446,
            "unit": "us/iter",
            "extra": "iterations: 1366428\ncpu: 0.5128624435389234 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 5.354693052215017,
            "unit": "us/iter",
            "extra": "iterations: 119333\ncpu: 5.354353757971299 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 4.918283862764626,
            "unit": "us/iter",
            "extra": "iterations: 132092\ncpu: 4.917943880022985 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Final Verification (3rd round)",
            "value": 5.759816266265627,
            "unit": "us/iter",
            "extra": "iterations: 107351\ncpu: 5.759024070572069 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Final Verification (3rd round)",
            "value": 5.502410903623955,
            "unit": "us/iter",
            "extra": "iterations: 103085\ncpu: 5.501789736625009 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 1st Message (1st round)",
            "value": 20527.585647053405,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20524.89667647037 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 1st Message (1st round)",
            "value": 20526.905970588334,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20524.21067647087 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.6322663995969259,
            "unit": "us/iter",
            "extra": "iterations: 1114067\ncpu: 0.6322313325859101 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.6323932388215223,
            "unit": "us/iter",
            "extra": "iterations: 1111256\ncpu: 0.632339808288986 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 353.3134440523382,
            "unit": "us/iter",
            "extra": "iterations: 1984\ncpu: 353.2629616935718 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 352.9057976791318,
            "unit": "us/iter",
            "extra": "iterations: 1982\ncpu: 352.86966246216497 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Final Verification (3rd round)",
            "value": 43798.32318750232,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43791.17868750271 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Final Verification (3rd round)",
            "value": 43984.42568748351,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43979.24281250098 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Prover",
            "value": 987.7915991559535,
            "unit": "us/iter",
            "extra": "iterations: 711\ncpu: 983.0193699015373 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Prover",
            "value": 903.909721862987,
            "unit": "us/iter",
            "extra": "iterations: 773\ncpu: 903.8134049159215 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Verifier",
            "value": 763.1367294630776,
            "unit": "us/iter",
            "extra": "iterations: 913\ncpu: 763.0686812705908 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Verifier",
            "value": 4526.463461037235,
            "unit": "us/iter",
            "extra": "iterations: 154\ncpu: 4525.856987012733 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Prover",
            "value": 77.2185672802479,
            "unit": "us/iter",
            "extra": "iterations: 8747\ncpu: 77.20952040699687 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Verifier",
            "value": 138.3867915752111,
            "unit": "us/iter",
            "extra": "iterations: 5009\ncpu: 138.3789774406075 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Prover/3",
            "value": 2739.834666666782,
            "unit": "us/iter",
            "extra": "iterations: 261\ncpu: 2722.5730306513456 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Verify/3",
            "value": 1074.776101226642,
            "unit": "us/iter",
            "extra": "iterations: 652\ncpu: 1074.686055214779 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Prover",
            "value": 78.12011551705709,
            "unit": "us/iter",
            "extra": "iterations: 9003\ncpu: 78.10962490280689 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Verifier",
            "value": 142.43236284448795,
            "unit": "us/iter",
            "extra": "iterations: 4936\ncpu: 142.43023460292082 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Prover/3",
            "value": 256.0286935425289,
            "unit": "us/iter",
            "extra": "iterations: 2741\ncpu: 256.02042539218405 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Verify/3",
            "value": 370.1447293112265,
            "unit": "us/iter",
            "extra": "iterations: 1873\ncpu: 370.13130966363957 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Prover/3",
            "value": 4298.390449102028,
            "unit": "us/iter",
            "extra": "iterations: 167\ncpu: 4265.336029940197 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Verify/3",
            "value": 1004.7155512822112,
            "unit": "us/iter",
            "extra": "iterations: 702\ncpu: 997.8113319088303 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Prover",
            "value": 23578.664966665503,
            "unit": "us/iter",
            "extra": "iterations: 30\ncpu: 23574.325033333327 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Verifier",
            "value": 22610.251548397377,
            "unit": "us/iter",
            "extra": "iterations: 31\ncpu: 22609.525193549798 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Prover",
            "value": 92440.68737501721,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 92440.03424999648 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Verifier",
            "value": 87974.61662499017,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 87971.15299999803 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Prover",
            "value": 90873.18062501027,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 90867.49337500066 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Verifier",
            "value": 183693.188999996,
            "unit": "us/iter",
            "extra": "iterations: 4\ncpu: 183679.54149999833 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Prover",
            "value": 331698.72899998154,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 331691.9820000237 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Verifier",
            "value": 17891.43062500216,
            "unit": "us/iter",
            "extra": "iterations: 40\ncpu: 17890.738725000687 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Prover",
            "value": 48161.80707143758,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 48156.29628571319 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Verifier",
            "value": 98839.56442860966,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 98833.07042857073 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Prover",
            "value": 404963.3710001217,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 404914.9399999976 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Verifier",
            "value": 117302.64166665923,
            "unit": "us/iter",
            "extra": "iterations: 6\ncpu: 117288.86016666707 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Prover",
            "value": 387477.3500001538,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 387448.707499999 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Verifier",
            "value": 127750.36100001671,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 127741.78299999904 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Prover",
            "value": 297892.707499841,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 297865.0725000023 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Verifier",
            "value": 298809.1054999131,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 298783.03350000124 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "1ce3207b3b3faecc210dca970f37a47061234eee",
          "message": "chore: Add benchmark for vartime curve operations (#21)",
          "timestamp": "2025-05-07T08:04:24-07:00",
          "tree_id": "7432baf39e3175583d0954415e1ac60801dc7e43",
          "url": "https://github.com/coinbase/cb-mpc/commit/1ce3207b3b3faecc210dca970f37a47061234eee"
        },
        "date": 1746632566532,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "ZK/Batch-DL/Prover/3/1",
            "value": 1369.052535019258,
            "unit": "us/iter",
            "extra": "iterations: 514\ncpu: 1369.0109357976655 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/1",
            "value": 1254.913064516224,
            "unit": "us/iter",
            "extra": "iterations: 558\ncpu: 1254.7232831541223 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Prover",
            "value": 1752.5715508689484,
            "unit": "us/iter",
            "extra": "iterations: 403\ncpu: 1752.4825657568235 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Prover",
            "value": 1661.4341976470741,
            "unit": "us/iter",
            "extra": "iterations: 425\ncpu: 1661.2727011764705 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Prover",
            "value": 4119.195866666236,
            "unit": "us/iter",
            "extra": "iterations: 165\ncpu: 4119.045066666667 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Prover",
            "value": 4094.3187126432586,
            "unit": "us/iter",
            "extra": "iterations: 174\ncpu: 4093.7537758620715 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Prover",
            "value": 12902.370625003608,
            "unit": "us/iter",
            "extra": "iterations: 56\ncpu: 12901.924232142852 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Prover",
            "value": 12882.23124137842,
            "unit": "us/iter",
            "extra": "iterations: 58\ncpu: 12770.561034482766 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/1",
            "value": 1476.7995776399716,
            "unit": "us/iter",
            "extra": "iterations: 483\ncpu: 1476.7681180124207 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/1",
            "value": 6210.853830357418,
            "unit": "us/iter",
            "extra": "iterations: 112\ncpu: 6210.42777678571 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Verifier",
            "value": 2231.9434240504465,
            "unit": "us/iter",
            "extra": "iterations: 316\ncpu: 2231.84762341772 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Verifier",
            "value": 14735.36978723461,
            "unit": "us/iter",
            "extra": "iterations: 47\ncpu: 14734.523276595739 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Verifier",
            "value": 5568.251032788093,
            "unit": "us/iter",
            "extra": "iterations: 122\ncpu: 5567.6471147541 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Verifier",
            "value": 48913.47435714774,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 48912.56378571425 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Verifier",
            "value": 28137.487439998946,
            "unit": "us/iter",
            "extra": "iterations: 25\ncpu: 28135.267759999962 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Verifier",
            "value": 273159.48799999506,
            "unit": "us/iter",
            "extra": "iterations: 3\ncpu: 273151.9476666667 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Verifier's Challenge (1st round)",
            "value": 0.5003776435865418,
            "unit": "us/iter",
            "extra": "iterations: 1398006\ncpu: 0.5003199492706025 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Prover Message (2nd round)",
            "value": 9745.855041665487,
            "unit": "us/iter",
            "extra": "iterations: 72\ncpu: 9744.952708333334 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Final Verification",
            "value": 9239.023773334338,
            "unit": "us/iter",
            "extra": "iterations: 75\ncpu: 9238.71270666666 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 1st Message (1st round)",
            "value": 33301.67671428864,
            "unit": "us/iter",
            "extra": "iterations: 21\ncpu: 33300.827809524 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Verifier's Challenge (2nd round)",
            "value": 1.9885589991633366,
            "unit": "us/iter",
            "extra": "iterations: 352505\ncpu: 1.9885079927944236 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 2nd Message (3rd round)",
            "value": 351.4602565526238,
            "unit": "us/iter",
            "extra": "iterations: 1984\ncpu: 351.4489299395212 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Final Verification (3rd round)",
            "value": 35124.84764999044,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35123.899299999546 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 1st Message (1st round)",
            "value": 35666.96630000479,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35664.48250000036 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Verifier's Challenge (2nd round)",
            "value": 0.5903074218875888,
            "unit": "us/iter",
            "extra": "iterations: 1189886\ncpu: 0.5902799789223495 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 2nd Message (3rd round)",
            "value": 704.7473145163505,
            "unit": "us/iter",
            "extra": "iterations: 992\ncpu: 704.7040584677437 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two Paillier Equal / Final Verification (3rd round)",
            "value": 73592.94200003003,
            "unit": "us/iter",
            "extra": "iterations: 9\ncpu: 73584.67588888906 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 1st Message (1st round)",
            "value": 129608.75540002235,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129593.44039999791 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 1st Message (1st round)",
            "value": 129558.04099992747,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129545.92560000152 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.5156967066947448,
            "unit": "us/iter",
            "extra": "iterations: 1370538\ncpu: 0.5156505328564603 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.5123260727867749,
            "unit": "us/iter",
            "extra": "iterations: 1370053\ncpu: 0.5122543514739946 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 6.39223945608249,
            "unit": "us/iter",
            "extra": "iterations: 117224\ncpu: 6.39166225346363 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 5.864007380000658,
            "unit": "us/iter",
            "extra": "iterations: 100000\ncpu: 5.863350820000051 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Final Verification (3rd round)",
            "value": 5.0541229366412495,
            "unit": "us/iter",
            "extra": "iterations: 106380\ncpu: 5.053157360406242 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Final Verification (3rd round)",
            "value": 5.45216051999887,
            "unit": "us/iter",
            "extra": "iterations: 100000\ncpu: 5.451576799999884 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 1st Message (1st round)",
            "value": 20458.63664706945,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20456.27111764712 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 1st Message (1st round)",
            "value": 20440.946941174418,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20440.131264705615 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.6442289167404867,
            "unit": "us/iter",
            "extra": "iterations: 1095407\ncpu: 0.6441677796471971 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.6619399185160456,
            "unit": "us/iter",
            "extra": "iterations: 1093465\ncpu: 0.6618870663441435 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 353.7675272727111,
            "unit": "us/iter",
            "extra": "iterations: 1980\ncpu: 353.75566818181665 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 353.3726434345152,
            "unit": "us/iter",
            "extra": "iterations: 1980\ncpu: 353.33781969696776 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Final Verification (3rd round)",
            "value": 43868.12393750006,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43861.97306250139 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Final Verification (3rd round)",
            "value": 43849.32874998526,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43846.422625001935 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Prover",
            "value": 1000.0510638295551,
            "unit": "us/iter",
            "extra": "iterations: 705\ncpu: 995.7084723404355 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Prover",
            "value": 928.8365869282393,
            "unit": "us/iter",
            "extra": "iterations: 765\ncpu: 924.0392627451037 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Verifier",
            "value": 758.904284940298,
            "unit": "us/iter",
            "extra": "iterations: 923\ncpu: 758.8446381364895 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Verifier",
            "value": 4490.517070513679,
            "unit": "us/iter",
            "extra": "iterations: 156\ncpu: 4490.09973076915 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Prover",
            "value": 77.0694762376088,
            "unit": "us/iter",
            "extra": "iterations: 9090\ncpu: 77.06058591858819 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Verifier",
            "value": 139.0551243868642,
            "unit": "us/iter",
            "extra": "iterations: 5097\ncpu: 139.0391516578462 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Prover/3",
            "value": 2767.185207843189,
            "unit": "us/iter",
            "extra": "iterations: 255\ncpu: 2766.7546745096483 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Verify/3",
            "value": 1069.8359709484023,
            "unit": "us/iter",
            "extra": "iterations: 654\ncpu: 1069.6878669725422 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Prover",
            "value": 77.67328268829198,
            "unit": "us/iter",
            "extra": "iterations: 9017\ncpu: 77.66340268382012 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Verifier",
            "value": 139.34263003882762,
            "unit": "us/iter",
            "extra": "iterations: 4887\ncpu: 139.3405979128325 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Prover/3",
            "value": 255.21037320576315,
            "unit": "us/iter",
            "extra": "iterations: 2717\ncpu: 255.18215973499034 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Verify/3",
            "value": 367.27262930135936,
            "unit": "us/iter",
            "extra": "iterations: 1918\ncpu: 367.22897393117273 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Prover/3",
            "value": 4315.965787880101,
            "unit": "us/iter",
            "extra": "iterations: 165\ncpu: 4297.957109090613 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Verify/3",
            "value": 994.9290014230835,
            "unit": "us/iter",
            "extra": "iterations: 703\ncpu: 994.7995903270966 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Prover",
            "value": 23448.95296666133,
            "unit": "us/iter",
            "extra": "iterations: 30\ncpu: 23446.191366665895 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Verifier",
            "value": 22560.543774195517,
            "unit": "us/iter",
            "extra": "iterations: 31\ncpu: 22560.63419354764 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Prover",
            "value": 92465.50624999372,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 92455.77600000132 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Verifier",
            "value": 88128.70437498076,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 88120.89587500083 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Prover",
            "value": 90730.78662498802,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 90726.62637499462 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Verifier",
            "value": 184144.5982499863,
            "unit": "us/iter",
            "extra": "iterations: 4\ncpu: 184141.2807499978 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Prover",
            "value": 331780.6484999437,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 331741.32700000313 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Verifier",
            "value": 17487.211975003447,
            "unit": "us/iter",
            "extra": "iterations: 40\ncpu: 17485.494574999906 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Prover",
            "value": 48168.92228570526,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 48164.81221428554 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Verifier",
            "value": 99118.00442854916,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 99104.79614285349 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Prover",
            "value": 382394.8154999926,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 382387.07549999165 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Verifier",
            "value": 117029.78116666902,
            "unit": "us/iter",
            "extra": "iterations: 6\ncpu: 117008.74883333275 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Prover",
            "value": 387641.31599987193,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 387605.7020000019 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Verifier",
            "value": 127032.9848000074,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 127026.81700000085 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Prover",
            "value": 296392.73800012236,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 296393.7055000088 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Verifier",
            "value": 297945.1014998631,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 297932.5489999951 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "9883f8dd9df8cae9d1c98c9dbee9fbecc9b61977",
          "message": "build: Use clang20 and clang‑format (#22)\n\n- Switch default compiler to Clang/Clang++ 20 and install clang‑format v20 in Docker\n  - Note that the necessity of `-Wno-error=deprecated-declarations` is because newer Clang surface the warning. The warning only appears in using google-test.\n- Reformat the entire codebase with clang‑format v20 (normalize lambda arrows, loop bodies, remove stray includes).\n- Reinforce constant‑time guarantees by inlining mask logic and inserting small assembly barriers in core modules. Note that some added barriers are necessary when we switch the compiler to Clang. The barriers prevent some optimizations that breaks the constant-time property.\n- Other minor changes: tidy up CMake options, `.gitignore`, and README.",
          "timestamp": "2025-05-07T09:46:19-07:00",
          "tree_id": "f2eb86e4d51a7656f8c948616d9c1ea305637822",
          "url": "https://github.com/coinbase/cb-mpc/commit/9883f8dd9df8cae9d1c98c9dbee9fbecc9b61977"
        },
        "date": 1746638654427,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "ZK/Batch-DL/Prover/3/1",
            "value": 1347.672412109091,
            "unit": "us/iter",
            "extra": "iterations: 512\ncpu: 1347.5048398437502 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/1",
            "value": 1244.4606464283327,
            "unit": "us/iter",
            "extra": "iterations: 560\ncpu: 1244.3035642857149 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Prover",
            "value": 1724.43057142864,
            "unit": "us/iter",
            "extra": "iterations: 406\ncpu: 1724.2315197044338 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Prover",
            "value": 1631.019851508053,
            "unit": "us/iter",
            "extra": "iterations: 431\ncpu: 1630.896220417633 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Prover",
            "value": 4200.295185628529,
            "unit": "us/iter",
            "extra": "iterations: 167\ncpu: 4176.237029940118 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Prover",
            "value": 4129.82276571321,
            "unit": "us/iter",
            "extra": "iterations: 175\ncpu: 4101.158131428575 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Prover",
            "value": 12959.526396551562,
            "unit": "us/iter",
            "extra": "iterations: 58\ncpu: 12857.843310344824 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Prover",
            "value": 12752.369309088283,
            "unit": "us/iter",
            "extra": "iterations: 55\ncpu: 12628.895327272732 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/1",
            "value": 1461.6806192467175,
            "unit": "us/iter",
            "extra": "iterations: 478\ncpu: 1461.5897928870306 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/1",
            "value": 6286.751819819377,
            "unit": "us/iter",
            "extra": "iterations: 111\ncpu: 6286.166891891893 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Verifier",
            "value": 2235.157403174526,
            "unit": "us/iter",
            "extra": "iterations: 315\ncpu: 2234.98077142857 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Verifier",
            "value": 14946.365063833777,
            "unit": "us/iter",
            "extra": "iterations: 47\ncpu: 14944.455978723412 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Verifier",
            "value": 5642.040792001354,
            "unit": "us/iter",
            "extra": "iterations: 125\ncpu: 5641.660560000005 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Verifier",
            "value": 49509.910499994345,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 49505.22814285715 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Verifier",
            "value": 28620.157639998066,
            "unit": "us/iter",
            "extra": "iterations: 25\ncpu: 28617.682279999954 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Verifier",
            "value": 278037.13566663646,
            "unit": "us/iter",
            "extra": "iterations: 3\ncpu: 278019.5903333329 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Verifier's Challenge (1st round)",
            "value": 0.502391900323825,
            "unit": "us/iter",
            "extra": "iterations: 1381512\ncpu: 0.502333415127772 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Prover Message (2nd round)",
            "value": 9743.849236110058,
            "unit": "us/iter",
            "extra": "iterations: 72\ncpu: 9743.104208333303 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Final Verification",
            "value": 9284.051479999107,
            "unit": "us/iter",
            "extra": "iterations: 75\ncpu: 9283.132666666726 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 1st Message (1st round)",
            "value": 33274.34061905132,
            "unit": "us/iter",
            "extra": "iterations: 21\ncpu: 33271.94100000005 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Verifier's Challenge (2nd round)",
            "value": 2.0343205201441044,
            "unit": "us/iter",
            "extra": "iterations: 344827\ncpu: 2.03419933473886 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 2nd Message (3rd round)",
            "value": 352.0569803922128,
            "unit": "us/iter",
            "extra": "iterations: 1989\ncpu: 352.0246993463998 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Final Verification (3rd round)",
            "value": 35145.876800015685,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35142.91294999978 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 1st Message (1st round)",
            "value": 35571.914799993465,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35569.17585000008 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Verifier's Challenge (2nd round)",
            "value": 0.5996953450952449,
            "unit": "us/iter",
            "extra": "iterations: 1163418\ncpu: 0.599649256758966 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 2nd Message (3rd round)",
            "value": 707.7484747984755,
            "unit": "us/iter",
            "extra": "iterations: 992\ncpu: 707.6809052419306 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two Paillier Equal / Final Verification (3rd round)",
            "value": 73618.48555557217,
            "unit": "us/iter",
            "extra": "iterations: 9\ncpu: 73614.66833333453 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 1st Message (1st round)",
            "value": 129753.21639996764,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129744.25400000201 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 1st Message (1st round)",
            "value": 129379.13599998865,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129368.66699999997 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.5214371051788894,
            "unit": "us/iter",
            "extra": "iterations: 1314361\ncpu: 0.5213817931298919 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.5238286071549403,
            "unit": "us/iter",
            "extra": "iterations: 1349152\ncpu: 0.5238048166551933 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 6.3775096789558265,
            "unit": "us/iter",
            "extra": "iterations: 100889\ncpu: 6.377195521811237 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 7.083309215046453,
            "unit": "us/iter",
            "extra": "iterations: 128160\ncpu: 7.0830822019350785 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Final Verification (3rd round)",
            "value": 5.678199343423192,
            "unit": "us/iter",
            "extra": "iterations: 100826\ncpu: 5.677655733640056 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Final Verification (3rd round)",
            "value": 7.33532538396008,
            "unit": "us/iter",
            "extra": "iterations: 135105\ncpu: 7.335024913955678 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 1st Message (1st round)",
            "value": 20505.069588232476,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20504.038529411413 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 1st Message (1st round)",
            "value": 20441.32232352989,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20440.267735294274 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.6439765743525259,
            "unit": "us/iter",
            "extra": "iterations: 1089575\ncpu: 0.6439106963724274 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.6444005405507918,
            "unit": "us/iter",
            "extra": "iterations: 1084819\ncpu: 0.6443847406802552 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 353.9010937658123,
            "unit": "us/iter",
            "extra": "iterations: 1973\ncpu: 353.886068423702 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 354.2609494181769,
            "unit": "us/iter",
            "extra": "iterations: 1977\ncpu: 354.23119271624125 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Final Verification (3rd round)",
            "value": 43791.31568751404,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43788.367187499236 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Final Verification (3rd round)",
            "value": 43805.92199999001,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43804.50668750058 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Prover",
            "value": 994.1966756378467,
            "unit": "us/iter",
            "extra": "iterations: 706\ncpu: 994.1074957506413 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Prover",
            "value": 921.0853494764063,
            "unit": "us/iter",
            "extra": "iterations: 764\ncpu: 920.741202879569 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Verifier",
            "value": 761.8615854188013,
            "unit": "us/iter",
            "extra": "iterations: 919\ncpu: 761.7614581066443 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Verifier",
            "value": 4572.408509803235,
            "unit": "us/iter",
            "extra": "iterations: 153\ncpu: 4571.839555555673 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Prover",
            "value": 77.18409295154787,
            "unit": "us/iter",
            "extra": "iterations: 9080\ncpu: 77.17174030836425 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Verifier",
            "value": 138.9668901318228,
            "unit": "us/iter",
            "extra": "iterations: 5006\ncpu: 138.95226048742052 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Prover/3",
            "value": 2739.074164706193,
            "unit": "us/iter",
            "extra": "iterations: 255\ncpu: 2733.038709803783 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Verify/3",
            "value": 1074.622945038095,
            "unit": "us/iter",
            "extra": "iterations: 655\ncpu: 1074.5816091603324 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Prover",
            "value": 77.73314127055204,
            "unit": "us/iter",
            "extra": "iterations: 9004\ncpu: 77.7302890937342 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Verifier",
            "value": 141.00335181058736,
            "unit": "us/iter",
            "extra": "iterations: 4943\ncpu: 140.9899765324638 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Prover/3",
            "value": 255.1756190301902,
            "unit": "us/iter",
            "extra": "iterations: 2743\ncpu: 255.16489135982982 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Verify/3",
            "value": 368.8931812366466,
            "unit": "us/iter",
            "extra": "iterations: 1876\ncpu: 368.8690303838019 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Prover/3",
            "value": 4241.6309447840285,
            "unit": "us/iter",
            "extra": "iterations: 163\ncpu: 4241.495380367905 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Verify/3",
            "value": 996.3342722064167,
            "unit": "us/iter",
            "extra": "iterations: 698\ncpu: 996.2210845272064 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Prover",
            "value": 23559.422566677313,
            "unit": "us/iter",
            "extra": "iterations: 30\ncpu: 23558.15410000067 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Verifier",
            "value": 22621.574258063185,
            "unit": "us/iter",
            "extra": "iterations: 31\ncpu: 22620.170129031183 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Prover",
            "value": 92479.46950000596,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 92476.18662499946 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Verifier",
            "value": 87959.3462500452,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 87953.41474999674 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Prover",
            "value": 90766.71949998171,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 90760.75999999489 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Verifier",
            "value": 184426.46850007804,
            "unit": "us/iter",
            "extra": "iterations: 4\ncpu: 184417.94150000135 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Prover",
            "value": 331947.73450009053,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 331943.6564999876 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Verifier",
            "value": 17438.406049996047,
            "unit": "us/iter",
            "extra": "iterations: 40\ncpu: 17436.81362499956 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Prover",
            "value": 48218.72720000707,
            "unit": "us/iter",
            "extra": "iterations: 15\ncpu: 48213.29153333333 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Verifier",
            "value": 98909.97942855912,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 98905.50357143217 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Prover",
            "value": 382351.5835001672,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 382327.45100000896 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Verifier",
            "value": 116881.80799994066,
            "unit": "us/iter",
            "extra": "iterations: 6\ncpu: 116878.45999999998 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Prover",
            "value": 387088.9515001181,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 387070.5574999818 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Verifier",
            "value": 127383.31979999202,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 127375.71299999216 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Prover",
            "value": 299204.735000103,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 299176.3124999807 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Verifier",
            "value": 297967.3154998181,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 297946.32149997825 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "188008098+stepsecurity-app[bot]@users.noreply.github.com",
            "name": "stepsecurity-app[bot]",
            "username": "stepsecurity-app[bot]"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "245fe04db8aa92cfd13c5d9143d3380af40f5adc",
          "message": "[StepSecurity] Apply security best practices (#27)\n\nSigned-off-by: StepSecurity Bot <bot@stepsecurity.io>\nCo-authored-by: stepsecurity-app[bot] <188008098+stepsecurity-app[bot]@users.noreply.github.com>",
          "timestamp": "2025-06-13T07:35:38-07:00",
          "tree_id": "df9a8bf7fb70e122e8a652ea168a71ba0f89f859",
          "url": "https://github.com/coinbase/cb-mpc/commit/245fe04db8aa92cfd13c5d9143d3380af40f5adc"
        },
        "date": 1749827762324,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "ZK/Batch-DL/Prover/3/1",
            "value": 1383.8800178567517,
            "unit": "us/iter",
            "extra": "iterations: 504\ncpu: 1383.794871031746 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/1",
            "value": 1282.1733135432942,
            "unit": "us/iter",
            "extra": "iterations: 539\ncpu: 1282.1049220779223 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Prover",
            "value": 1779.9136020404167,
            "unit": "us/iter",
            "extra": "iterations: 392\ncpu: 1779.7815714285719 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Prover",
            "value": 1686.266154761792,
            "unit": "us/iter",
            "extra": "iterations: 420\ncpu: 1684.011238095237 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Prover",
            "value": 4411.643128834416,
            "unit": "us/iter",
            "extra": "iterations: 163\ncpu: 4394.173134969323 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Prover",
            "value": 4362.258518293546,
            "unit": "us/iter",
            "extra": "iterations: 164\ncpu: 4333.390170731709 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Prover",
            "value": 13590.146180004012,
            "unit": "us/iter",
            "extra": "iterations: 50\ncpu: 13474.112500000005 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Prover",
            "value": 13249.332719997255,
            "unit": "us/iter",
            "extra": "iterations: 50\ncpu: 13103.257520000006 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/1",
            "value": 1456.0456037737672,
            "unit": "us/iter",
            "extra": "iterations: 477\ncpu: 1455.8891740041925 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/1",
            "value": 6399.130128440047,
            "unit": "us/iter",
            "extra": "iterations: 109\ncpu: 6398.691110091733 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Verifier",
            "value": 2249.766529412199,
            "unit": "us/iter",
            "extra": "iterations: 323\ncpu: 2249.638839009287 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Verifier",
            "value": 15123.701195653153,
            "unit": "us/iter",
            "extra": "iterations: 46\ncpu: 15122.847565217366 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Verifier",
            "value": 5630.8345520010325,
            "unit": "us/iter",
            "extra": "iterations: 125\ncpu: 5630.5646 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Verifier",
            "value": 49828.203642846354,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 49824.36192857139 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Verifier",
            "value": 28120.985999994446,
            "unit": "us/iter",
            "extra": "iterations: 25\ncpu: 28119.44751999995 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Verifier",
            "value": 278643.8346667334,
            "unit": "us/iter",
            "extra": "iterations: 3\ncpu: 278624.4206666666 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Verifier's Challenge (1st round)",
            "value": 0.736833249231671,
            "unit": "us/iter",
            "extra": "iterations: 953207\ncpu: 0.7368033659005851 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Prover Message (2nd round)",
            "value": 9739.102375003009,
            "unit": "us/iter",
            "extra": "iterations: 72\ncpu: 9738.461472222287 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Final Verification",
            "value": 9277.039160000035,
            "unit": "us/iter",
            "extra": "iterations: 75\ncpu: 9276.435093333266 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 1st Message (1st round)",
            "value": 33304.56161904208,
            "unit": "us/iter",
            "extra": "iterations: 21\ncpu: 33302.45452380947 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Verifier's Challenge (2nd round)",
            "value": 2.965356288716398,
            "unit": "us/iter",
            "extra": "iterations: 236662\ncpu: 2.965150881848378 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 2nd Message (3rd round)",
            "value": 352.12280736264165,
            "unit": "us/iter",
            "extra": "iterations: 1983\ncpu: 352.1150574886525 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Final Verification (3rd round)",
            "value": 35139.64425001177,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35137.14699999966 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 1st Message (1st round)",
            "value": 35643.54504999301,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35641.70125000032 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Verifier's Challenge (2nd round)",
            "value": 0.8247814244008437,
            "unit": "us/iter",
            "extra": "iterations: 837152\ncpu: 0.8247517655097325 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 2nd Message (3rd round)",
            "value": 705.9751995966728,
            "unit": "us/iter",
            "extra": "iterations: 992\ncpu: 705.9089153225747 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two Paillier Equal / Final Verification (3rd round)",
            "value": 73590.94666667766,
            "unit": "us/iter",
            "extra": "iterations: 9\ncpu: 73583.87211111031 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 1st Message (1st round)",
            "value": 129625.1387999291,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129613.2861999979 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 1st Message (1st round)",
            "value": 129607.2474000539,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129597.76320000175 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.7470111609766508,
            "unit": "us/iter",
            "extra": "iterations: 936746\ncpu: 0.7469499202558596 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.7476828321304976,
            "unit": "us/iter",
            "extra": "iterations: 938276\ncpu: 0.7476402913428324 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 6.513098684262717,
            "unit": "us/iter",
            "extra": "iterations: 129433\ncpu: 6.5126261231679745 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 6.917408612287117,
            "unit": "us/iter",
            "extra": "iterations: 125170\ncpu: 6.916813909083455 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Final Verification (3rd round)",
            "value": 5.387693036839347,
            "unit": "us/iter",
            "extra": "iterations: 100644\ncpu: 5.3872393684671325 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Final Verification (3rd round)",
            "value": 6.135222603841311,
            "unit": "us/iter",
            "extra": "iterations: 121413\ncpu: 6.134797361073469 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 1st Message (1st round)",
            "value": 20548.016470590537,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20547.19061764759 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 1st Message (1st round)",
            "value": 20469.76176470975,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20468.74861764787 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.8867478634910119,
            "unit": "us/iter",
            "extra": "iterations: 790893\ncpu: 0.8866991514654947 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.8861060555731576,
            "unit": "us/iter",
            "extra": "iterations: 791717\ncpu: 0.8860256632104376 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 353.7357414931568,
            "unit": "us/iter",
            "extra": "iterations: 1969\ncpu: 353.7162991366062 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 353.7405545454352,
            "unit": "us/iter",
            "extra": "iterations: 1980\ncpu: 353.71364848486735 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Final Verification (3rd round)",
            "value": 43786.0398124883,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43784.69950000152 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Final Verification (3rd round)",
            "value": 43939.80162498679,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43934.75299999849 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Prover",
            "value": 1014.9881536226925,
            "unit": "us/iter",
            "extra": "iterations: 690\ncpu: 1014.9353869564948 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Prover",
            "value": 940.4241631015409,
            "unit": "us/iter",
            "extra": "iterations: 748\ncpu: 940.3365762031984 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Verifier",
            "value": 771.5066747250903,
            "unit": "us/iter",
            "extra": "iterations: 910\ncpu: 771.4106769230466 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Verifier",
            "value": 4600.794920530244,
            "unit": "us/iter",
            "extra": "iterations: 151\ncpu: 4600.360000000102 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Prover",
            "value": 77.76950011063,
            "unit": "us/iter",
            "extra": "iterations: 9040\ncpu: 77.75850929203219 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Verifier",
            "value": 137.47208339904094,
            "unit": "us/iter",
            "extra": "iterations: 5072\ncpu: 137.46572929810398 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Prover/3",
            "value": 2781.790164659747,
            "unit": "us/iter",
            "extra": "iterations: 249\ncpu: 2775.893811244933 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Verify/3",
            "value": 1082.8950815385513,
            "unit": "us/iter",
            "extra": "iterations: 650\ncpu: 1081.5953446153624 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Prover",
            "value": 77.98804094609953,
            "unit": "us/iter",
            "extra": "iterations: 8963\ncpu: 77.98464621220474 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Verifier",
            "value": 141.85504997944707,
            "unit": "us/iter",
            "extra": "iterations: 4862\ncpu: 141.8491507610082 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Prover/3",
            "value": 256.1261259697969,
            "unit": "us/iter",
            "extra": "iterations: 2707\ncpu: 255.84812855559457 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Verify/3",
            "value": 369.81982091152776,
            "unit": "us/iter",
            "extra": "iterations: 1865\ncpu: 369.79792171583034 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Prover/3",
            "value": 4338.9505766882485,
            "unit": "us/iter",
            "extra": "iterations: 163\ncpu: 4323.023730061242 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Verify/3",
            "value": 1003.1678998569002,
            "unit": "us/iter",
            "extra": "iterations: 699\ncpu: 1003.072024320431 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Prover",
            "value": 23413.58783332907,
            "unit": "us/iter",
            "extra": "iterations: 30\ncpu: 23412.167600001037 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Verifier",
            "value": 22556.318129024858,
            "unit": "us/iter",
            "extra": "iterations: 31\ncpu: 22554.77167741923 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Prover",
            "value": 92655.58537498464,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 92649.90724999933 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Verifier",
            "value": 87829.58412501785,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 87823.58612499763 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Prover",
            "value": 90897.30800002371,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 90885.5473749952 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Verifier",
            "value": 183854.7192500073,
            "unit": "us/iter",
            "extra": "iterations: 4\ncpu: 183836.26624999748 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Prover",
            "value": 332109.5134999723,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 332080.76300002175 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Verifier",
            "value": 17546.063874999618,
            "unit": "us/iter",
            "extra": "iterations: 40\ncpu: 17544.397224999386 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Prover",
            "value": 48135.15906668423,
            "unit": "us/iter",
            "extra": "iterations: 15\ncpu: 48131.4745333331 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Verifier",
            "value": 98894.66728567901,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 98888.89442856908 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Prover",
            "value": 383049.9600001076,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 383030.85750001745 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Verifier",
            "value": 116859.32916664872,
            "unit": "us/iter",
            "extra": "iterations: 6\ncpu: 116850.51766666751 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Prover",
            "value": 386668.61349997815,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 386657.1689999887 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Verifier",
            "value": 127811.92720003673,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 127798.67399999601 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Prover",
            "value": 297015.25150017004,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 296997.2024999947 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Verifier",
            "value": 297822.4109999701,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 297805.7269999965 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "047a698ada9a5dc4671ea9b63a767d43aa276725",
          "message": "chore: cleanup unused codes in buf128_t and buf256_t (#26)",
          "timestamp": "2025-06-14T09:38:51-07:00",
          "tree_id": "99a4ea4ac8f2945d771597aa4c902a6e9d6ce726",
          "url": "https://github.com/coinbase/cb-mpc/commit/047a698ada9a5dc4671ea9b63a767d43aa276725"
        },
        "date": 1749921461059,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "ZK/Batch-DL/Prover/3/1",
            "value": 1376.4004114170712,
            "unit": "us/iter",
            "extra": "iterations: 508\ncpu: 1376.358375984252 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/1",
            "value": 1274.434164557372,
            "unit": "us/iter",
            "extra": "iterations: 553\ncpu: 1274.3465587703436 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Prover",
            "value": 1767.0043092270464,
            "unit": "us/iter",
            "extra": "iterations: 401\ncpu: 1766.870331670823 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Prover",
            "value": 1651.0912797194537,
            "unit": "us/iter",
            "extra": "iterations: 429\ncpu: 1651.0299720279718 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Prover",
            "value": 4121.877766468055,
            "unit": "us/iter",
            "extra": "iterations: 167\ncpu: 4121.505365269463 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Prover",
            "value": 4018.9789526626964,
            "unit": "us/iter",
            "extra": "iterations: 169\ncpu: 4018.6764615384604 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Prover",
            "value": 13042.208622638733,
            "unit": "us/iter",
            "extra": "iterations: 53\ncpu: 13041.20864150942 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Prover",
            "value": 12841.924500004812,
            "unit": "us/iter",
            "extra": "iterations: 56\ncpu: 12840.95417857142 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/1",
            "value": 1494.9279525863262,
            "unit": "us/iter",
            "extra": "iterations: 464\ncpu: 1489.1068146551715 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/1",
            "value": 6197.42142477654,
            "unit": "us/iter",
            "extra": "iterations: 113\ncpu: 6197.171946902649 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Verifier",
            "value": 2280.641954838493,
            "unit": "us/iter",
            "extra": "iterations: 310\ncpu: 2280.4102193548383 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Verifier",
            "value": 14681.102085113649,
            "unit": "us/iter",
            "extra": "iterations: 47\ncpu: 14680.353702127655 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Verifier",
            "value": 5486.062024392891,
            "unit": "us/iter",
            "extra": "iterations: 123\ncpu: 5485.623918699176 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Verifier",
            "value": 48635.70992857344,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 48633.48214285715 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Verifier",
            "value": 28542.613041679488,
            "unit": "us/iter",
            "extra": "iterations: 24\ncpu: 28538.644791666604 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Verifier",
            "value": 272150.0486666931,
            "unit": "us/iter",
            "extra": "iterations: 3\ncpu: 272133.7440000001 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Verifier's Challenge (1st round)",
            "value": 0.7451048265933303,
            "unit": "us/iter",
            "extra": "iterations: 948433\ncpu: 0.7450465546854662 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Prover Message (2nd round)",
            "value": 9765.240222217624,
            "unit": "us/iter",
            "extra": "iterations: 72\ncpu: 9764.837555555534 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Final Verification",
            "value": 9282.720239995495,
            "unit": "us/iter",
            "extra": "iterations: 75\ncpu: 9281.772106666704 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 1st Message (1st round)",
            "value": 33380.04661903445,
            "unit": "us/iter",
            "extra": "iterations: 21\ncpu: 33376.63600000019 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Verifier's Challenge (2nd round)",
            "value": 3.0103989320901814,
            "unit": "us/iter",
            "extra": "iterations: 233353\ncpu: 3.010301667430914 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 2nd Message (3rd round)",
            "value": 353.92112361811843,
            "unit": "us/iter",
            "extra": "iterations: 1990\ncpu: 353.87336231155984 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Final Verification (3rd round)",
            "value": 35122.49924999651,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35121.85935000005 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 1st Message (1st round)",
            "value": 35610.166950004896,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35604.16264999944 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Verifier's Challenge (2nd round)",
            "value": 0.8472954825260396,
            "unit": "us/iter",
            "extra": "iterations: 825240\ncpu: 0.847244748194471 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 2nd Message (3rd round)",
            "value": 704.8316398388703,
            "unit": "us/iter",
            "extra": "iterations: 994\ncpu: 704.7911307847023 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two Paillier Equal / Final Verification (3rd round)",
            "value": 73685.06033334495,
            "unit": "us/iter",
            "extra": "iterations: 9\ncpu: 73680.33222222279 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 1st Message (1st round)",
            "value": 129764.13139995202,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129751.233799999 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 1st Message (1st round)",
            "value": 130386.42400006209,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 130374.76339999898 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.7586485291937491,
            "unit": "us/iter",
            "extra": "iterations: 924697\ncpu: 0.758560820463364 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.7596699427351072,
            "unit": "us/iter",
            "extra": "iterations: 924482\ncpu: 0.7595971030263536 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 5.615853071795778,
            "unit": "us/iter",
            "extra": "iterations: 106957\ncpu: 5.615298157203194 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 5.991600350993714,
            "unit": "us/iter",
            "extra": "iterations: 188608\ncpu: 5.991244883568058 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Final Verification (3rd round)",
            "value": 5.858954889999949,
            "unit": "us/iter",
            "extra": "iterations: 100000\ncpu: 5.858552960000054 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Final Verification (3rd round)",
            "value": 5.614674065056609,
            "unit": "us/iter",
            "extra": "iterations: 106825\ncpu: 5.6138746360871865 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 1st Message (1st round)",
            "value": 20490.64570589026,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20489.475823529134 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 1st Message (1st round)",
            "value": 20471.88076470347,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20469.776117647096 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.9089129798118077,
            "unit": "us/iter",
            "extra": "iterations: 778509\ncpu: 0.9088786128355674 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.9040834523253938,
            "unit": "us/iter",
            "extra": "iterations: 774562\ncpu: 0.9040028583896379 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 353.6103713996353,
            "unit": "us/iter",
            "extra": "iterations: 1979\ncpu: 353.5762854977167 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 354.9649142281094,
            "unit": "us/iter",
            "extra": "iterations: 1982\ncpu: 354.9188884964543 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Final Verification (3rd round)",
            "value": 43948.95081250638,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43944.22174999946 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Final Verification (3rd round)",
            "value": 43771.02231251229,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43769.18568750199 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Prover",
            "value": 1017.7837631951859,
            "unit": "us/iter",
            "extra": "iterations: 701\ncpu: 1009.6362853066769 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Prover",
            "value": 934.599974767808,
            "unit": "us/iter",
            "extra": "iterations: 753\ncpu: 927.5534794157103 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Verifier",
            "value": 791.5337640451469,
            "unit": "us/iter",
            "extra": "iterations: 890\ncpu: 791.4990382022517 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Verifier",
            "value": 4490.376993591128,
            "unit": "us/iter",
            "extra": "iterations: 156\ncpu: 4489.832570512656 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Prover",
            "value": 79.10936128374682,
            "unit": "us/iter",
            "extra": "iterations: 8849\ncpu: 79.10593988021562 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Verifier",
            "value": 140.69118142214205,
            "unit": "us/iter",
            "extra": "iterations: 5049\ncpu: 140.67361338879303 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Prover/3",
            "value": 2866.5329051371286,
            "unit": "us/iter",
            "extra": "iterations: 253\ncpu: 2833.359786561264 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Verify/3",
            "value": 1099.2190815047927,
            "unit": "us/iter",
            "extra": "iterations: 638\ncpu: 1099.0389545454034 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Prover",
            "value": 79.55961535821478,
            "unit": "us/iter",
            "extra": "iterations: 8738\ncpu: 79.55148855573232 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Verifier",
            "value": 143.35528777128303,
            "unit": "us/iter",
            "extra": "iterations: 4792\ncpu: 143.3364000417324 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Prover/3",
            "value": 259.3234247132014,
            "unit": "us/iter",
            "extra": "iterations: 2703\ncpu: 259.29303329632404 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Verify/3",
            "value": 372.374419109601,
            "unit": "us/iter",
            "extra": "iterations: 1842\ncpu: 372.33315309444976 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Prover/3",
            "value": 4305.128518292737,
            "unit": "us/iter",
            "extra": "iterations: 164\ncpu: 4304.82112804879 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Verify/3",
            "value": 1021.0598413390685,
            "unit": "us/iter",
            "extra": "iterations: 687\ncpu: 1020.8946666666119 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Prover",
            "value": 23417.117733333725,
            "unit": "us/iter",
            "extra": "iterations: 30\ncpu: 23413.580033333874 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Verifier",
            "value": 22532.37745161084,
            "unit": "us/iter",
            "extra": "iterations: 31\ncpu: 22529.78348387145 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Prover",
            "value": 92833.26625001109,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 92825.71862500078 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Verifier",
            "value": 89229.1992500418,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 89217.37174999577 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Prover",
            "value": 91021.45737500678,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 91013.57800000187 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Verifier",
            "value": 184191.4642500342,
            "unit": "us/iter",
            "extra": "iterations: 4\ncpu: 184168.75924999942 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Prover",
            "value": 331745.5989999871,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 331694.5574999863 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Verifier",
            "value": 17631.930549998742,
            "unit": "us/iter",
            "extra": "iterations: 40\ncpu: 17629.173299999933 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Prover",
            "value": 48173.631000008754,
            "unit": "us/iter",
            "extra": "iterations: 15\ncpu: 48167.17860000305 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Verifier",
            "value": 98940.27828568142,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 98931.99857143438 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Prover",
            "value": 383344.4340000369,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 383327.0129999846 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Verifier",
            "value": 117054.4724999824,
            "unit": "us/iter",
            "extra": "iterations: 6\ncpu: 117048.20350000016 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Prover",
            "value": 388534.97699983564,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 388510.32399998076 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Verifier",
            "value": 127902.24039999885,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 127886.3175999959 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Prover",
            "value": 297220.8994999619,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 297162.58950000454 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Verifier",
            "value": 300663.17749992775,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 300599.08499998756 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "ee61f1f2ceb91985da7ff5f69d002b9cd8fdc6e8",
          "message": "fix: enable flexible t-of-n cryptogrpahic operations (#29)\n\n- Adds a missing feature in threshold dkg, where only parties 0-t could participate and no one else\n- Changes the job to accept party names (pname_t) as opposed to pids\n- Changes the go wrappers accordingly",
          "timestamp": "2025-06-17T09:27:04-06:00",
          "tree_id": "aaa6f62b84e3a69cb3441c51a2fa9979416abc5d",
          "url": "https://github.com/coinbase/cb-mpc/commit/ee61f1f2ceb91985da7ff5f69d002b9cd8fdc6e8"
        },
        "date": 1750176293735,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "ZK/Batch-DL/Prover/3/1",
            "value": 1363.5405362038,
            "unit": "us/iter",
            "extra": "iterations: 511\ncpu: 1363.4738590998045 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/1",
            "value": 1263.4254829443787,
            "unit": "us/iter",
            "extra": "iterations: 557\ncpu: 1263.3898940754043 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Prover",
            "value": 1751.2693075002517,
            "unit": "us/iter",
            "extra": "iterations: 400\ncpu: 1751.2061724999994 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Prover",
            "value": 1653.427510637999,
            "unit": "us/iter",
            "extra": "iterations: 423\ncpu: 1653.377177304965 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Prover",
            "value": 4128.620289156423,
            "unit": "us/iter",
            "extra": "iterations: 166\ncpu: 4128.449831325301 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Prover",
            "value": 4123.665735631158,
            "unit": "us/iter",
            "extra": "iterations: 174\ncpu: 4123.551350574717 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Prover",
            "value": 12893.561035712408,
            "unit": "us/iter",
            "extra": "iterations: 56\ncpu: 12892.496142857146 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Prover",
            "value": 12628.818947369406,
            "unit": "us/iter",
            "extra": "iterations: 57\ncpu: 12628.309929824567 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/1",
            "value": 1459.8674110173197,
            "unit": "us/iter",
            "extra": "iterations: 472\ncpu: 1459.7237521186446 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/1",
            "value": 6198.372407081189,
            "unit": "us/iter",
            "extra": "iterations: 113\ncpu: 6197.926460176991 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Verifier",
            "value": 2227.708170347185,
            "unit": "us/iter",
            "extra": "iterations: 317\ncpu: 2227.6078769716064 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Verifier",
            "value": 14666.14672916459,
            "unit": "us/iter",
            "extra": "iterations: 48\ncpu: 14665.000770833363 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Verifier",
            "value": 5502.0933464542695,
            "unit": "us/iter",
            "extra": "iterations: 127\ncpu: 5501.657606299214 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Verifier",
            "value": 48719.03614285397,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 48713.932928571456 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Verifier",
            "value": 27528.171399990242,
            "unit": "us/iter",
            "extra": "iterations: 25\ncpu: 27527.508399999977 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Verifier",
            "value": 272190.3793333998,
            "unit": "us/iter",
            "extra": "iterations: 3\ncpu: 272174.98500000004 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Verifier's Challenge (1st round)",
            "value": 0.7446758927867866,
            "unit": "us/iter",
            "extra": "iterations: 940482\ncpu: 0.7446414912778732 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Prover Message (2nd round)",
            "value": 9784.651388890275,
            "unit": "us/iter",
            "extra": "iterations: 72\ncpu: 9784.258847222165 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Final Verification",
            "value": 9255.799533336054,
            "unit": "us/iter",
            "extra": "iterations: 75\ncpu: 9255.433346666658 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 1st Message (1st round)",
            "value": 37498.44633334543,
            "unit": "us/iter",
            "extra": "iterations: 21\ncpu: 37495.57757142865 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Verifier's Challenge (2nd round)",
            "value": 2.962857531127073,
            "unit": "us/iter",
            "extra": "iterations: 236213\ncpu: 2.962737072049393 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 2nd Message (3rd round)",
            "value": 351.08665169101783,
            "unit": "us/iter",
            "extra": "iterations: 1981\ncpu: 351.0651433619408 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Final Verification (3rd round)",
            "value": 35303.843700012294,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35301.733600000065 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 1st Message (1st round)",
            "value": 35735.80254999342,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35733.93265000036 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Verifier's Challenge (2nd round)",
            "value": 0.8497403642952173,
            "unit": "us/iter",
            "extra": "iterations: 817195\ncpu: 0.8497236375650916 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 2nd Message (3rd round)",
            "value": 707.7549045226905,
            "unit": "us/iter",
            "extra": "iterations: 995\ncpu: 707.6760532663227 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two Paillier Equal / Final Verification (3rd round)",
            "value": 73528.51933329576,
            "unit": "us/iter",
            "extra": "iterations: 9\ncpu: 73525.80822222243 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 1st Message (1st round)",
            "value": 130969.20039997713,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 130961.20079999878 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 1st Message (1st round)",
            "value": 130637.76240005609,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 130636.81820000285 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.7551454301320771,
            "unit": "us/iter",
            "extra": "iterations: 930935\ncpu: 0.755105632509273 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.7502383699307767,
            "unit": "us/iter",
            "extra": "iterations: 938365\ncpu: 0.7502080288587072 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 6.670877147906802,
            "unit": "us/iter",
            "extra": "iterations: 113657\ncpu: 6.670398954749687 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 5.998481500000707,
            "unit": "us/iter",
            "extra": "iterations: 100000\ncpu: 5.998268449999956 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Final Verification (3rd round)",
            "value": 5.727895165556074,
            "unit": "us/iter",
            "extra": "iterations: 105824\ncpu: 5.727927795207231 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Final Verification (3rd round)",
            "value": 7.098704980220788,
            "unit": "us/iter",
            "extra": "iterations: 92526\ncpu: 7.09844069774966 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 1st Message (1st round)",
            "value": 20444.79558823214,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20443.619794117836 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 1st Message (1st round)",
            "value": 20667.236470591673,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20664.947882353015 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.8783337277948551,
            "unit": "us/iter",
            "extra": "iterations: 799403\ncpu: 0.8783263297736035 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.8765178063108262,
            "unit": "us/iter",
            "extra": "iterations: 796740\ncpu: 0.8764807477972875 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 353.2420030333419,
            "unit": "us/iter",
            "extra": "iterations: 1978\ncpu: 353.1859671385267 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 353.77784840828286,
            "unit": "us/iter",
            "extra": "iterations: 1979\ncpu: 353.7609898938817 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Final Verification (3rd round)",
            "value": 43855.92587499332,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43851.59662500016 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Final Verification (3rd round)",
            "value": 44931.552500003134,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 44927.471999997695 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Prover",
            "value": 1014.737047413767,
            "unit": "us/iter",
            "extra": "iterations: 696\ncpu: 1007.3185531608873 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Prover",
            "value": 939.0507839258885,
            "unit": "us/iter",
            "extra": "iterations: 759\ncpu: 930.9215942028887 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Verifier",
            "value": 769.0223775051525,
            "unit": "us/iter",
            "extra": "iterations: 898\ncpu: 767.4392071269718 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Verifier",
            "value": 4487.147455127646,
            "unit": "us/iter",
            "extra": "iterations: 156\ncpu: 4487.0214166668375 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Prover",
            "value": 77.63076417710339,
            "unit": "us/iter",
            "extra": "iterations: 9011\ncpu: 77.6274311397151 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Verifier",
            "value": 140.51487797554904,
            "unit": "us/iter",
            "extra": "iterations: 4999\ncpu: 140.49931246249744 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Prover/3",
            "value": 2765.680529644044,
            "unit": "us/iter",
            "extra": "iterations: 253\ncpu: 2765.341498023706 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Verify/3",
            "value": 1078.5961794479233,
            "unit": "us/iter",
            "extra": "iterations: 652\ncpu: 1078.4503450919944 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Prover",
            "value": 78.75009833484184,
            "unit": "us/iter",
            "extra": "iterations: 8888\ncpu: 78.74379804229935 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Verifier",
            "value": 144.22472478180836,
            "unit": "us/iter",
            "extra": "iterations: 4927\ncpu: 144.212716866252 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Prover/3",
            "value": 257.22049432858984,
            "unit": "us/iter",
            "extra": "iterations: 2733\ncpu: 257.2047625320081 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Verify/3",
            "value": 371.27952431288253,
            "unit": "us/iter",
            "extra": "iterations: 1892\ncpu: 371.2392103594047 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Prover/3",
            "value": 4342.428549383234,
            "unit": "us/iter",
            "extra": "iterations: 162\ncpu: 4297.320617284039 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Verify/3",
            "value": 999.2787579252638,
            "unit": "us/iter",
            "extra": "iterations: 694\ncpu: 999.1711008645093 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Prover",
            "value": 23446.487333330875,
            "unit": "us/iter",
            "extra": "iterations: 30\ncpu: 23445.724733333387 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Verifier",
            "value": 22553.283225798008,
            "unit": "us/iter",
            "extra": "iterations: 31\ncpu: 22551.12599999987 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Prover",
            "value": 92552.03962499081,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 92545.94649999603 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Verifier",
            "value": 87793.97000000699,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 87785.23937500182 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Prover",
            "value": 90620.65674999075,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 90613.86900000201 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Verifier",
            "value": 183606.404499983,
            "unit": "us/iter",
            "extra": "iterations: 4\ncpu: 183599.16100000363 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Prover",
            "value": 334862.2345001786,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 334855.5229999875 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Verifier",
            "value": 17544.405025000742,
            "unit": "us/iter",
            "extra": "iterations: 40\ncpu: 17543.836874999386 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Prover",
            "value": 48392.8688000257,
            "unit": "us/iter",
            "extra": "iterations: 15\ncpu: 48390.587800001114 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Verifier",
            "value": 98909.51599995139,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 98902.05642856893 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Prover",
            "value": 382417.237999789,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 382382.35049999505 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Verifier",
            "value": 116855.41599998336,
            "unit": "us/iter",
            "extra": "iterations: 6\ncpu: 116841.18733333547 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Prover",
            "value": 387247.72000000486,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 387228.0710000098 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Verifier",
            "value": 126955.40120002988,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 126941.7390000058 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Prover",
            "value": 297250.8980001294,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 297202.16149999603 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Verifier",
            "value": 297447.04949985136,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 297430.43799999217 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "c7b470ad4d0d4cc313931317d202ffa5abe033b5",
          "message": "feat: Revamp Go wrapper (#31)\n\nAlso, we refactor the job_t's to facilitate the MPC protocol testing. In particular, it includes\n- introduces a data_transport_interface_t to decouple the MPC protocol logic from the underlying data transport mechanism. (Previously done in `job_session_t`, which is suboptimal)\n- The base job_mp_t class is refactored to use this interface, making it agnostic to the specific network implementation.\n- job_session_mp_t and its subclasses are simplified, bridging the generic job_mp_t with the parallel network_t.\n- Tests have been updated to reflect this new, more modular design.\n- Rename `job_session_t` to `job_parallel_t`\n\n* refactor: revamp Go wrapper\n\nNow the go wrapper conform with the standard Go convention,\nwith api, internal directories, etc.\nWe also introduce more wrappers as APIs around\n- curve\n- curve points\n- scalar\n- ECDSA-MP protocols\n- ECDSA-2P protocols\n- EdDSA-MP Protocols\n- ZK-DL Protocols",
          "timestamp": "2025-06-25T07:19:39-07:00",
          "tree_id": "1aae96b441550c242fe91717805c5fad5b547dc7",
          "url": "https://github.com/coinbase/cb-mpc/commit/c7b470ad4d0d4cc313931317d202ffa5abe033b5"
        },
        "date": 1750863421175,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "ZK/Batch-DL/Prover/3/1",
            "value": 1344.440468085461,
            "unit": "us/iter",
            "extra": "iterations: 517\ncpu: 1344.3200502901354 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/1",
            "value": 1251.4810975181904,
            "unit": "us/iter",
            "extra": "iterations: 564\ncpu: 1247.9146187943263 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Prover",
            "value": 1703.1826374696607,
            "unit": "us/iter",
            "extra": "iterations: 411\ncpu: 1701.029102189781 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Prover",
            "value": 1599.9919011491506,
            "unit": "us/iter",
            "extra": "iterations: 435\ncpu: 1596.1187287356313 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Prover",
            "value": 3969.5477151162563,
            "unit": "us/iter",
            "extra": "iterations: 172\ncpu: 3942.937668604649 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Prover",
            "value": 3947.0074525118503,
            "unit": "us/iter",
            "extra": "iterations: 179\ncpu: 3921.7565810055908 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Prover",
            "value": 12181.981333333168,
            "unit": "us/iter",
            "extra": "iterations: 60\ncpu: 12083.153516666674 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Prover",
            "value": 12886.19474074174,
            "unit": "us/iter",
            "extra": "iterations: 54\ncpu: 12758.207685185187 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/1",
            "value": 1465.9162075467766,
            "unit": "us/iter",
            "extra": "iterations: 477\ncpu: 1465.7057903563955 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/1",
            "value": 6182.559353982896,
            "unit": "us/iter",
            "extra": "iterations: 113\ncpu: 6182.263991150431 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Verifier",
            "value": 2200.4753498439877,
            "unit": "us/iter",
            "extra": "iterations: 323\ncpu: 2200.2855015479877 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Verifier",
            "value": 14661.642270832925,
            "unit": "us/iter",
            "extra": "iterations: 48\ncpu: 14660.895770833331 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Verifier",
            "value": 5641.874247998203,
            "unit": "us/iter",
            "extra": "iterations: 125\ncpu: 5641.125935999994 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Verifier",
            "value": 48685.54392857861,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 48684.015071428534 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Verifier",
            "value": 28411.335639993922,
            "unit": "us/iter",
            "extra": "iterations: 25\ncpu: 28409.959640000012 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Verifier",
            "value": 272404.60833339364,
            "unit": "us/iter",
            "extra": "iterations: 3\ncpu: 272391.21766666643 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Verifier's Challenge (1st round)",
            "value": 0.7598401395457235,
            "unit": "us/iter",
            "extra": "iterations: 923568\ncpu: 0.759839762746219 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Prover Message (2nd round)",
            "value": 9733.944055552809,
            "unit": "us/iter",
            "extra": "iterations: 72\ncpu: 9732.8004166666 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Final Verification",
            "value": 9230.116173330316,
            "unit": "us/iter",
            "extra": "iterations: 75\ncpu: 9229.702893333259 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 1st Message (1st round)",
            "value": 33324.4109523808,
            "unit": "us/iter",
            "extra": "iterations: 21\ncpu: 33323.36838095258 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Verifier's Challenge (2nd round)",
            "value": 3.0233391631622935,
            "unit": "us/iter",
            "extra": "iterations: 228766\ncpu: 3.023225544879929 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 2nd Message (3rd round)",
            "value": 352.7441043652987,
            "unit": "us/iter",
            "extra": "iterations: 1993\ncpu: 352.73547967887964 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Final Verification (3rd round)",
            "value": 35139.5547500033,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35137.21194999988 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 1st Message (1st round)",
            "value": 35687.81894998665,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35686.76605000008 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Verifier's Challenge (2nd round)",
            "value": 0.8596669306140264,
            "unit": "us/iter",
            "extra": "iterations: 824660\ncpu: 0.8596261125797168 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 2nd Message (3rd round)",
            "value": 703.1765698490228,
            "unit": "us/iter",
            "extra": "iterations: 995\ncpu: 703.1771407035128 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two Paillier Equal / Final Verification (3rd round)",
            "value": 73627.3655555427,
            "unit": "us/iter",
            "extra": "iterations: 9\ncpu: 73622.81199999958 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 1st Message (1st round)",
            "value": 129680.62280006052,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129668.74759999881 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 1st Message (1st round)",
            "value": 129560.15780000598,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129556.9807999982 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.7631129649484276,
            "unit": "us/iter",
            "extra": "iterations: 920613\ncpu: 0.7630832868968693 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.7656608152768555,
            "unit": "us/iter",
            "extra": "iterations: 915566\ncpu: 0.765634659871608 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 6.2408280199997535,
            "unit": "us/iter",
            "extra": "iterations: 100000\ncpu: 6.240206480000269 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 5.836292939861717,
            "unit": "us/iter",
            "extra": "iterations: 142391\ncpu: 5.835884950593755 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Final Verification (3rd round)",
            "value": 6.282179359484766,
            "unit": "us/iter",
            "extra": "iterations: 114658\ncpu: 6.281555643740681 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Final Verification (3rd round)",
            "value": 5.545250327762016,
            "unit": "us/iter",
            "extra": "iterations: 119751\ncpu: 5.545043156215848 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 1st Message (1st round)",
            "value": 20466.893352931947,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20466.37597058855 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 1st Message (1st round)",
            "value": 20498.279176469143,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20496.678705882234 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.9134274162411349,
            "unit": "us/iter",
            "extra": "iterations: 766728\ncpu: 0.913401601350166 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.9142043134203629,
            "unit": "us/iter",
            "extra": "iterations: 762133\ncpu: 0.9140929352225854 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 354.62188109767436,
            "unit": "us/iter",
            "extra": "iterations: 1968\ncpu: 354.60286991870186 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 353.9646044512444,
            "unit": "us/iter",
            "extra": "iterations: 1977\ncpu: 353.91043045016744 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Final Verification (3rd round)",
            "value": 43731.84481249837,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43726.55337499864 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Final Verification (3rd round)",
            "value": 43727.62681248332,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43724.69225000231 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Prover",
            "value": 990.2141352111512,
            "unit": "us/iter",
            "extra": "iterations: 710\ncpu: 990.0788760562687 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Prover",
            "value": 930.000349081564,
            "unit": "us/iter",
            "extra": "iterations: 762\ncpu: 927.2804160104449 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Verifier",
            "value": 774.84454934201,
            "unit": "us/iter",
            "extra": "iterations: 912\ncpu: 772.7822686403391 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Verifier",
            "value": 4490.723929489622,
            "unit": "us/iter",
            "extra": "iterations: 156\ncpu: 4483.465038461532 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Prover",
            "value": 77.59620820886973,
            "unit": "us/iter",
            "extra": "iterations: 9039\ncpu: 77.58635014935525 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Verifier",
            "value": 137.38820063758166,
            "unit": "us/iter",
            "extra": "iterations: 5019\ncpu: 137.38457541342297 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Prover/3",
            "value": 2708.8901322957536,
            "unit": "us/iter",
            "extra": "iterations: 257\ncpu: 2705.6097276263495 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Verify/3",
            "value": 1074.7752392635002,
            "unit": "us/iter",
            "extra": "iterations: 652\ncpu: 1074.735036809826 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Prover",
            "value": 78.70884523544265,
            "unit": "us/iter",
            "extra": "iterations: 8962\ncpu: 78.7034718812816 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Verifier",
            "value": 143.05851107012472,
            "unit": "us/iter",
            "extra": "iterations: 4878\ncpu: 143.0421080360729 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Prover/3",
            "value": 257.26850275431474,
            "unit": "us/iter",
            "extra": "iterations: 2723\ncpu: 257.26604002939183 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Verify/3",
            "value": 365.76015201280944,
            "unit": "us/iter",
            "extra": "iterations: 1888\ncpu: 365.719677436441 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Prover/3",
            "value": 4280.787078787949,
            "unit": "us/iter",
            "extra": "iterations: 165\ncpu: 4255.266969697101 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Verify/3",
            "value": 1007.705712643802,
            "unit": "us/iter",
            "extra": "iterations: 696\ncpu: 1007.6108750000614 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Prover",
            "value": 23507.37529998999,
            "unit": "us/iter",
            "extra": "iterations: 30\ncpu: 23505.238500001724 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Verifier",
            "value": 22611.68822580599,
            "unit": "us/iter",
            "extra": "iterations: 31\ncpu: 22609.646806452314 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Prover",
            "value": 92627.60137499981,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 92621.63862499762 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Verifier",
            "value": 87796.1728750165,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 87788.23274999326 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Prover",
            "value": 90707.88525002627,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 90696.01512499759 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Verifier",
            "value": 183906.2632500372,
            "unit": "us/iter",
            "extra": "iterations: 4\ncpu: 183891.2442499918 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Prover",
            "value": 334849.7464999127,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 334816.6874999947 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Verifier",
            "value": 17528.24424999062,
            "unit": "us/iter",
            "extra": "iterations: 40\ncpu: 17525.231575000078 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Prover",
            "value": 48190.31464285217,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 48188.57228571411 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Verifier",
            "value": 99243.37514283771,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 99238.52942857333 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Prover",
            "value": 382781.6519999487,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 382722.6660000065 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Verifier",
            "value": 116826.40033336611,
            "unit": "us/iter",
            "extra": "iterations: 6\ncpu: 116811.95133333707 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Prover",
            "value": 387103.5199999824,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 387079.81999999675 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Verifier",
            "value": 127311.17140001516,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 127300.13240000063 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Prover",
            "value": 297905.5124999377,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 297877.7699999853 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Verifier",
            "value": 297659.7469998978,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 297639.6164999926 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "056e00cc1daba5e7bd6b4655a11920a9c4fd1cb9",
          "message": "feat: Add msg size to Full-OT bench (#34)\n\nAlso fixed the bug where Full-OT time benches were accmulative.",
          "timestamp": "2025-07-09T07:00:00-07:00",
          "tree_id": "3e652f80af54ea162f520ddc683fea75111ce6e0",
          "url": "https://github.com/coinbase/cb-mpc/commit/056e00cc1daba5e7bd6b4655a11920a9c4fd1cb9"
        },
        "date": 1752071929885,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "ZK/Batch-DL/Prover/3/1",
            "value": 1369.865326172004,
            "unit": "us/iter",
            "extra": "iterations: 512\ncpu: 1369.7645703125002 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/1",
            "value": 1258.3934075401808,
            "unit": "us/iter",
            "extra": "iterations: 557\ncpu: 1258.2436624775585 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Prover",
            "value": 1730.9157475000347,
            "unit": "us/iter",
            "extra": "iterations: 400\ncpu: 1730.8554974999995 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Prover",
            "value": 1637.6320486115346,
            "unit": "us/iter",
            "extra": "iterations: 432\ncpu: 1637.4680023148148 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Prover",
            "value": 4101.061364161521,
            "unit": "us/iter",
            "extra": "iterations: 173\ncpu: 4100.676132947974 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Prover",
            "value": 3993.245277457138,
            "unit": "us/iter",
            "extra": "iterations: 173\ncpu: 3992.6292254335267 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Prover",
            "value": 12562.895472724862,
            "unit": "us/iter",
            "extra": "iterations: 55\ncpu: 12562.437709090913 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Prover",
            "value": 12385.804821430416,
            "unit": "us/iter",
            "extra": "iterations: 56\ncpu: 12385.159035714274 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/1",
            "value": 1493.4253212764002,
            "unit": "us/iter",
            "extra": "iterations: 470\ncpu: 1493.3837957446797 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/1",
            "value": 6193.813265485714,
            "unit": "us/iter",
            "extra": "iterations: 113\ncpu: 6193.40363716815 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Verifier",
            "value": 2241.8599419354227,
            "unit": "us/iter",
            "extra": "iterations: 310\ncpu: 2241.660438709676 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Verifier",
            "value": 14710.463333334663,
            "unit": "us/iter",
            "extra": "iterations: 48\ncpu: 14710.044062499983 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Verifier",
            "value": 5614.530460316957,
            "unit": "us/iter",
            "extra": "iterations: 126\ncpu: 5614.274269841266 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Verifier",
            "value": 48765.93500000581,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 48763.66235714296 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Verifier",
            "value": 28380.965079995804,
            "unit": "us/iter",
            "extra": "iterations: 25\ncpu: 28378.869679999993 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Verifier",
            "value": 272890.6053333352,
            "unit": "us/iter",
            "extra": "iterations: 3\ncpu: 272878.56966666685 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Verifier's Challenge (1st round)",
            "value": 0.7565415044088596,
            "unit": "us/iter",
            "extra": "iterations: 927793\ncpu: 0.756487486971772 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Prover Message (2nd round)",
            "value": 9742.230861111997,
            "unit": "us/iter",
            "extra": "iterations: 72\ncpu: 9741.535472222218 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Final Verification",
            "value": 9244.503279999966,
            "unit": "us/iter",
            "extra": "iterations: 75\ncpu: 9244.07702666675 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 1st Message (1st round)",
            "value": 33302.524571419264,
            "unit": "us/iter",
            "extra": "iterations: 21\ncpu: 33300.83276190468 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Verifier's Challenge (2nd round)",
            "value": 3.076470256676175,
            "unit": "us/iter",
            "extra": "iterations: 225849\ncpu: 3.07640160018421 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 2nd Message (3rd round)",
            "value": 351.1173207830626,
            "unit": "us/iter",
            "extra": "iterations: 1992\ncpu: 351.0867103413667 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Final Verification (3rd round)",
            "value": 35171.63994999919,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35168.86674999995 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 1st Message (1st round)",
            "value": 35589.290250004524,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35586.57274999959 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Verifier's Challenge (2nd round)",
            "value": 0.8588207189125682,
            "unit": "us/iter",
            "extra": "iterations: 818653\ncpu: 0.8588033244854696 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 2nd Message (3rd round)",
            "value": 705.5947645871754,
            "unit": "us/iter",
            "extra": "iterations: 994\ncpu: 705.551537223331 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two Paillier Equal / Final Verification (3rd round)",
            "value": 73639.3787777969,
            "unit": "us/iter",
            "extra": "iterations: 9\ncpu: 73631.91466666624 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 1st Message (1st round)",
            "value": 129573.32659998428,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129556.00239999968 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 1st Message (1st round)",
            "value": 129721.43840006538,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129703.11039999842 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.7682230617558383,
            "unit": "us/iter",
            "extra": "iterations: 914926\ncpu: 0.7682119045693332 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.7673487753839916,
            "unit": "us/iter",
            "extra": "iterations: 913674\ncpu: 0.7672499392562306 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 5.105770873008663,
            "unit": "us/iter",
            "extra": "iterations: 127078\ncpu: 5.105093013739585 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 6.481506039999658,
            "unit": "us/iter",
            "extra": "iterations: 100000\ncpu: 6.4810856299999395 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Final Verification (3rd round)",
            "value": 4.925513506521841,
            "unit": "us/iter",
            "extra": "iterations: 112131\ncpu: 4.92524149432362 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Final Verification (3rd round)",
            "value": 6.447942838493447,
            "unit": "us/iter",
            "extra": "iterations: 137890\ncpu: 6.447126941765094 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 1st Message (1st round)",
            "value": 20469.143176475936,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20468.152411764353 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 1st Message (1st round)",
            "value": 20465.375764709228,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20464.61167647017 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.9118732218114503,
            "unit": "us/iter",
            "extra": "iterations: 767143\ncpu: 0.9118285104602482 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.9079771793784,
            "unit": "us/iter",
            "extra": "iterations: 768866\ncpu: 0.9078962498016632 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 354.270236722404,
            "unit": "us/iter",
            "extra": "iterations: 1977\ncpu: 354.249998988367 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 353.9434714788796,
            "unit": "us/iter",
            "extra": "iterations: 1981\ncpu: 353.9235911155986 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Final Verification (3rd round)",
            "value": 43885.07056251001,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43881.22850000187 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Final Verification (3rd round)",
            "value": 43859.58143751623,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43858.621312498515 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Prover",
            "value": 999.4483124113199,
            "unit": "us/iter",
            "extra": "iterations: 701\ncpu: 999.2886961484077 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Prover",
            "value": 923.3918977719117,
            "unit": "us/iter",
            "extra": "iterations: 763\ncpu: 923.3167640891081 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Verifier",
            "value": 806.4618822870538,
            "unit": "us/iter",
            "extra": "iterations: 892\ncpu: 806.3963004483876 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Verifier",
            "value": 4501.165122579668,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4500.792780644983 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Prover",
            "value": 79.1153123014214,
            "unit": "us/iter",
            "extra": "iterations: 8812\ncpu: 79.10913231956947 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Verifier",
            "value": 140.1633477554561,
            "unit": "us/iter",
            "extra": "iterations: 4923\ncpu: 140.15125330082373 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Prover/3",
            "value": 3131.8509196795226,
            "unit": "us/iter",
            "extra": "iterations: 249\ncpu: 3131.5898674698333 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Verify/3",
            "value": 1097.4858151657797,
            "unit": "us/iter",
            "extra": "iterations: 633\ncpu: 1097.3604139020483 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Prover",
            "value": 79.44809016676219,
            "unit": "us/iter",
            "extra": "iterations: 8817\ncpu: 79.44275978223716 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Verifier",
            "value": 145.35332652212446,
            "unit": "us/iter",
            "extra": "iterations: 4796\ncpu: 145.32932631360126 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Prover/3",
            "value": 258.96336532050816,
            "unit": "us/iter",
            "extra": "iterations: 2699\ncpu: 258.9465935531801 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Verify/3",
            "value": 372.9478479533899,
            "unit": "us/iter",
            "extra": "iterations: 1881\ncpu: 372.88808931417196 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Prover/3",
            "value": 4274.522067483699,
            "unit": "us/iter",
            "extra": "iterations: 163\ncpu: 4274.1752883436975 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Verify/3",
            "value": 1021.8144410486688,
            "unit": "us/iter",
            "extra": "iterations: 687\ncpu: 1021.7225982532187 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Prover",
            "value": 23438.924966664366,
            "unit": "us/iter",
            "extra": "iterations: 30\ncpu: 23435.540833332878 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Verifier",
            "value": 22584.389096781735,
            "unit": "us/iter",
            "extra": "iterations: 31\ncpu: 22582.452258063768 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Prover",
            "value": 92480.21987497168,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 92476.46712500312 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Verifier",
            "value": 87851.93825002579,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 87846.06399999718 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Prover",
            "value": 93598.98462503224,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 93590.59725000663 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Verifier",
            "value": 183774.60399995017,
            "unit": "us/iter",
            "extra": "iterations: 4\ncpu: 183764.79775000122 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Prover",
            "value": 331785.91649993905,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 331766.81049999956 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Verifier",
            "value": 17474.9914500012,
            "unit": "us/iter",
            "extra": "iterations: 40\ncpu: 17474.11277499964 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Prover",
            "value": 54697.14149999163,
            "unit": "us/iter",
            "extra": "iterations: 10\ncpu: 54696.07549999864 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Verifier",
            "value": 103057.81085714573,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 103057.09314285747 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Prover",
            "value": 382204.3139998641,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 382186.5404999869 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Verifier",
            "value": 118376.7258333622,
            "unit": "us/iter",
            "extra": "iterations: 6\ncpu: 118374.6868333344 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Prover",
            "value": 387097.87000016146,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 387059.9274999904 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Verifier",
            "value": 128046.12480003925,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 128027.53140000506 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Prover",
            "value": 297292.57200006035,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 297282.07150000683 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Verifier",
            "value": 297573.6519999828,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 297569.02099998685 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "87906c7fd12c82d5fd999036dbe8610743f2c8aa",
          "message": "refactor: Crypto hardening and code cleanup (#36)\n\n- Removes numerous unused functions and code paths across the crypto modules (base, rsa, ec, ed25519).\n- Introduces input validation for deserializing containers (std::vector, bufs128_t) to guard against large allocation attacks.\n- Fixes an integer overflow vulnerability in the scr_inv modular inverse implementation.\n- Improves BN_CTX thread-local storage management by switching to scoped_ptr_t.\n- Strengthens the mod_t::coprime check by catching exceptions for non-invertible elements.\n- Adds a clear thread-safety warning to the main README.md.\n- Adds comprehensive unit tests for the coprime and scr_inv logic.",
          "timestamp": "2025-07-11T06:00:50-07:00",
          "tree_id": "8110280b666e0e9c1efc842ce35e49dbfa20eee5",
          "url": "https://github.com/coinbase/cb-mpc/commit/87906c7fd12c82d5fd999036dbe8610743f2c8aa"
        },
        "date": 1752241166692,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "ZK/Batch-DL/Prover/3/1",
            "value": 1380.834061386183,
            "unit": "us/iter",
            "extra": "iterations: 505\ncpu: 1380.8051920792082 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/1",
            "value": 1276.4823599999627,
            "unit": "us/iter",
            "extra": "iterations: 550\ncpu: 1276.251750909091 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Prover",
            "value": 1776.0620773196465,
            "unit": "us/iter",
            "extra": "iterations: 388\ncpu: 1775.9241984536077 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Prover",
            "value": 1668.3353723154246,
            "unit": "us/iter",
            "extra": "iterations: 419\ncpu: 1668.0156467780432 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Prover",
            "value": 4167.362875000164,
            "unit": "us/iter",
            "extra": "iterations: 168\ncpu: 4167.070333333332 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Prover",
            "value": 4131.1422215574585,
            "unit": "us/iter",
            "extra": "iterations: 167\ncpu: 4130.5035748503 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Prover",
            "value": 12833.157280702406,
            "unit": "us/iter",
            "extra": "iterations: 57\ncpu: 12832.876403508775 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Prover",
            "value": 13172.079018869346,
            "unit": "us/iter",
            "extra": "iterations: 53\ncpu: 13170.422245283016 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/1",
            "value": 1475.3057157676196,
            "unit": "us/iter",
            "extra": "iterations: 482\ncpu: 1475.2516307053963 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/1",
            "value": 6651.483633928836,
            "unit": "us/iter",
            "extra": "iterations: 112\ncpu: 6650.978401785714 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Verifier",
            "value": 2230.823025723483,
            "unit": "us/iter",
            "extra": "iterations: 311\ncpu: 2230.5623151125415 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Verifier",
            "value": 14784.295829790704,
            "unit": "us/iter",
            "extra": "iterations: 47\ncpu: 14782.586723404265 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Verifier",
            "value": 5609.164562500268,
            "unit": "us/iter",
            "extra": "iterations: 128\ncpu: 5608.364343749991 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Verifier",
            "value": 49026.41442856748,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 49025.35664285715 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Verifier",
            "value": 28348.458159998703,
            "unit": "us/iter",
            "extra": "iterations: 25\ncpu: 28345.751519999994 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Verifier",
            "value": 274834.2623332671,
            "unit": "us/iter",
            "extra": "iterations: 3\ncpu: 274802.66599999997 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Verifier's Challenge (1st round)",
            "value": 0.7665532819141212,
            "unit": "us/iter",
            "extra": "iterations: 912242\ncpu: 0.7664614247096705 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Prover Message (2nd round)",
            "value": 9743.093444443375,
            "unit": "us/iter",
            "extra": "iterations: 72\ncpu: 9742.395416666659 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Final Verification",
            "value": 9238.883200002118,
            "unit": "us/iter",
            "extra": "iterations: 75\ncpu: 9238.306213333279 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 1st Message (1st round)",
            "value": 33305.000809529396,
            "unit": "us/iter",
            "extra": "iterations: 21\ncpu: 33303.23047619051 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Verifier's Challenge (2nd round)",
            "value": 3.048810948076546,
            "unit": "us/iter",
            "extra": "iterations: 230762\ncpu: 3.0482576117385083 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 2nd Message (3rd round)",
            "value": 352.1254724171981,
            "unit": "us/iter",
            "extra": "iterations: 1994\ncpu: 352.0724563691106 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Final Verification (3rd round)",
            "value": 35151.483599997846,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35150.52109999956 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 1st Message (1st round)",
            "value": 35635.04264999438,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35633.61300000025 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Verifier's Challenge (2nd round)",
            "value": 0.8538700517015592,
            "unit": "us/iter",
            "extra": "iterations: 828214\ncpu: 0.8537604169936912 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 2nd Message (3rd round)",
            "value": 704.6408732391649,
            "unit": "us/iter",
            "extra": "iterations: 994\ncpu: 704.6119466800852 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two Paillier Equal / Final Verification (3rd round)",
            "value": 73801.62555556226,
            "unit": "us/iter",
            "extra": "iterations: 9\ncpu: 73789.54522222457 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 1st Message (1st round)",
            "value": 129859.6978000205,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129835.9784000013 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 1st Message (1st round)",
            "value": 129777.65999994517,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129774.17939999895 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.7696908597450285,
            "unit": "us/iter",
            "extra": "iterations: 908979\ncpu: 0.7696467586159895 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.7704230016616812,
            "unit": "us/iter",
            "extra": "iterations: 908630\ncpu: 0.7703645246139608 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 5.709087795404327,
            "unit": "us/iter",
            "extra": "iterations: 131852\ncpu: 5.70843218912118 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 6.742356299335683,
            "unit": "us/iter",
            "extra": "iterations: 97550\ncpu: 6.741356801640164 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Final Verification (3rd round)",
            "value": 5.130498673697222,
            "unit": "us/iter",
            "extra": "iterations: 123652\ncpu: 5.130128271277537 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Final Verification (3rd round)",
            "value": 7.458470007557916,
            "unit": "us/iter",
            "extra": "iterations: 123081\ncpu: 7.458040794273692 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 1st Message (1st round)",
            "value": 20485.488264699492,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20483.674411765078 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 1st Message (1st round)",
            "value": 20505.059441173966,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20501.057764706125 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.9046341099954842,
            "unit": "us/iter",
            "extra": "iterations: 777086\ncpu: 0.9044774413642845 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.9030877874761494,
            "unit": "us/iter",
            "extra": "iterations: 771978\ncpu: 0.9030228542782288 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 353.13357416757583,
            "unit": "us/iter",
            "extra": "iterations: 1982\ncpu: 353.0988567103963 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 354.24187967646,
            "unit": "us/iter",
            "extra": "iterations: 1978\ncpu: 354.0764504550319 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Final Verification (3rd round)",
            "value": 43834.125312514516,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43829.95143749824 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Final Verification (3rd round)",
            "value": 43808.58037498569,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43803.55049999807 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Prover",
            "value": 1013.7353909880043,
            "unit": "us/iter",
            "extra": "iterations: 688\ncpu: 1013.6484549417975 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Prover",
            "value": 934.4509156630281,
            "unit": "us/iter",
            "extra": "iterations: 747\ncpu: 934.2664658634466 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Verifier",
            "value": 772.9872929512727,
            "unit": "us/iter",
            "extra": "iterations: 908\ncpu: 772.9150198237685 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Verifier",
            "value": 4509.8782645166675,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4509.12287741953 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Prover",
            "value": 77.71895783998674,
            "unit": "us/iter",
            "extra": "iterations: 9037\ncpu: 77.70917837777613 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Verifier",
            "value": 139.36635994424637,
            "unit": "us/iter",
            "extra": "iterations: 5023\ncpu: 139.35276727054955 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Prover/3",
            "value": 2782.724374502645,
            "unit": "us/iter",
            "extra": "iterations: 251\ncpu: 2782.305430278745 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Verify/3",
            "value": 1081.2459383665268,
            "unit": "us/iter",
            "extra": "iterations: 649\ncpu: 1081.2239845916426 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Prover",
            "value": 78.40881170711924,
            "unit": "us/iter",
            "extra": "iterations: 8986\ncpu: 78.40374872022723 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Verifier",
            "value": 143.08327601069007,
            "unit": "us/iter",
            "extra": "iterations: 4873\ncpu: 143.07798727683578 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Prover/3",
            "value": 256.21118676460407,
            "unit": "us/iter",
            "extra": "iterations: 2720\ncpu: 256.17247867647063 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Verify/3",
            "value": 369.36551519906885,
            "unit": "us/iter",
            "extra": "iterations: 1908\ncpu: 369.357976415093 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Prover/3",
            "value": 4302.176087500698,
            "unit": "us/iter",
            "extra": "iterations: 160\ncpu: 4301.618775000194 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Verify/3",
            "value": 1004.5223419538893,
            "unit": "us/iter",
            "extra": "iterations: 696\ncpu: 1004.4518060344698 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Prover",
            "value": 23458.483866670576,
            "unit": "us/iter",
            "extra": "iterations: 30\ncpu: 23456.73510000097 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Verifier",
            "value": 22644.99309677053,
            "unit": "us/iter",
            "extra": "iterations: 31\ncpu: 22643.87058064512 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Prover",
            "value": 92717.23575000124,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 92708.25849999653 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Verifier",
            "value": 87896.13800001916,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 87891.02637500434 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Prover",
            "value": 90875.64374999602,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 90858.98400000048 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Verifier",
            "value": 184251.46525009949,
            "unit": "us/iter",
            "extra": "iterations: 4\ncpu: 184243.53275000272 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Prover",
            "value": 331738.44349994394,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 331725.9609999894 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Verifier",
            "value": 17442.287724998096,
            "unit": "us/iter",
            "extra": "iterations: 40\ncpu: 17440.736474999598 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Prover",
            "value": 48062.484599995514,
            "unit": "us/iter",
            "extra": "iterations: 15\ncpu: 48058.09880000046 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Verifier",
            "value": 98833.403000039,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 98821.4168571387 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Prover",
            "value": 382563.0689998434,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 382507.3120000013 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Verifier",
            "value": 117126.24749998213,
            "unit": "us/iter",
            "extra": "iterations: 6\ncpu: 117113.22599999601 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Prover",
            "value": 387219.9254999487,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 387171.5859999938 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Verifier",
            "value": 127610.34159993869,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 127599.47399999874 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Prover",
            "value": 296738.56649992557,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 296729.9314999821 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Verifier",
            "value": 297374.0835000172,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 297366.45799999905 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "cc4265d32ae59dab0c3917f7e39ac76fc44516a6",
          "message": "feat: Pluggable KEMs, Distributed Decryption, and Core Hardening (#45)\n\nThis commit introduces a major architectural refactor focused on cryptographic flexibility, API safety, and support for distributed operations. It incorporates several critical bug fixes related to concurrency, cryptographic correctness, and memory management. The public API has been updated, the CGO boundary has been hardened, and the developer experience has been improved.\n\n---\n## Key Architectural Changes\n\n### Pluggable, Instance-Based Architecture\nThe core API has been refactored from package-level functions to an **instance-based model**. Clients now create a handle (`NewPVE`) configured with a specific Key Encapsulation Mechanism (KEM) implementation. This is enabled by a new `mpc.KEM` Go interface and a generic `kem_aead_ciphertext_t` in C++, allowing different encryption backends to be plugged in seamlessly.\n\n### Interactive Distributed Decryption Protocol\nThe decryption process is now an **interactive, two-step protocol** for distributed environments, enhancing security by not requiring private keys to be co-located:\n1.  `AcPartyDecryptRow`: Each party generates a \"decryption share\" using its private key.\n2.  `AcAggregateToRestoreRow`: A new function aggregates shares from a quorum of parties to reconstruct the plaintext.\n\n### CGO Boundary Hardening & Memory Safety\nThe interface between Go and C++ has been overhauled to improve safety:\n* **No Leaked Handles**: The `go:linkname` pattern has been removed. Primitives (curve IDs, `[]byte`) are now passed instead of raw C++ pointers.\n* **Defensive Copying**: Data from Go is immediately copied into C++-owned buffers to prevent use-after-free errors from the Go GC (Fixes #42, #43).\n* **Memory Leak Fixed**: A memory leak in `hash32bit_for_zk_fischlin` was patched by correctly freeing the `EVP_MD_CTX` (Fixes #41).\n* **Clearer Ownership**: C++ message classes now own their data directly, improving clarity and memory safety.\n\n---\n## Additional Bug Fixes\n\n* **Deadlock Prevention**: A deadlock in parallel receive operations is now prevented. When a master thread encountered an error, it would leave worker threads blocked indefinitely. The fix ensures all waiting threads are properly notified on an error (Fixes #44).\n* **Corrected Public Key Reconstruction**: An uninitialized variable in the `reconstruct_pub_additive_shares` function could cause incorrect results. Shares are now correctly initialized to the curve's identity point (`infinity`), ensuring parties outside the quorum do not affect the sum (Fixes #38).\n* **ECDSA Test Fix**: The ECDSA test suite now works correctly with the `secp256k1` curve (Fixes #35).\n\n---\n## Developer Experience & Documentation\n\n* **Simplified Build Process**: A new `go_with_cpp.sh` wrapper script automatically rebuilds the C++ library as needed.\n* **Faster Filtered Tests**: The `make test-go` target is now significantly faster when using a filter.\n* **Cleaner Test Logs**: A new `testutil.TSilence` helper suppresses C++ `stderr` during tests that intentionally trigger errors.\n* **Documentation**: The documentation for Go wrapper compilation has been corrected.\n\nFixes #35, #38, #41, #42, #43, #44",
          "timestamp": "2025-09-25T07:20:22-07:00",
          "tree_id": "3f24c9802b3339b2fa1c1ee9fbe1802ac960ab45",
          "url": "https://github.com/coinbase/cb-mpc/commit/cc4265d32ae59dab0c3917f7e39ac76fc44516a6"
        },
        "date": 1758812361781,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "ZK/Batch-DL/Prover/3/1",
            "value": 1354.1615818884495,
            "unit": "us/iter",
            "extra": "iterations: 519\ncpu: 1354.0984662813103 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/1",
            "value": 1246.1352627866368,
            "unit": "us/iter",
            "extra": "iterations: 567\ncpu: 1246.0301816578487 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Prover",
            "value": 1710.9148272506645,
            "unit": "us/iter",
            "extra": "iterations: 411\ncpu: 1710.7212262773712 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Prover",
            "value": 1608.643812785254,
            "unit": "us/iter",
            "extra": "iterations: 438\ncpu: 1608.4220433789949 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Prover",
            "value": 3979.2751954023674,
            "unit": "us/iter",
            "extra": "iterations: 174\ncpu: 3978.972925287355 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Prover",
            "value": 3878.064394445523,
            "unit": "us/iter",
            "extra": "iterations: 180\ncpu: 3877.5965444444464 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Prover",
            "value": 12066.746266665026,
            "unit": "us/iter",
            "extra": "iterations: 60\ncpu: 12065.649116666662 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Prover",
            "value": 12295.964786886305,
            "unit": "us/iter",
            "extra": "iterations: 61\ncpu: 12294.367180327872 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/1",
            "value": 1452.4999338839523,
            "unit": "us/iter",
            "extra": "iterations: 484\ncpu: 1452.3394504132232 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/1",
            "value": 6318.974445455503,
            "unit": "us/iter",
            "extra": "iterations: 110\ncpu: 6318.297090909087 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Verifier",
            "value": 2244.4173396822393,
            "unit": "us/iter",
            "extra": "iterations: 315\ncpu: 2244.142571428573 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Verifier",
            "value": 15062.979276598662,
            "unit": "us/iter",
            "extra": "iterations: 47\ncpu: 15061.802808510662 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Verifier",
            "value": 5598.036185484679,
            "unit": "us/iter",
            "extra": "iterations: 124\ncpu: 5597.611016129029 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Verifier",
            "value": 49889.12950000213,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 49884.52028571417 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Verifier",
            "value": 28142.40255999721,
            "unit": "us/iter",
            "extra": "iterations: 25\ncpu: 28139.202240000002 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Verifier",
            "value": 279378.2146666217,
            "unit": "us/iter",
            "extra": "iterations: 3\ncpu: 279358.527 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Verifier's Challenge (1st round)",
            "value": 0.7407682618664435,
            "unit": "us/iter",
            "extra": "iterations: 920867\ncpu: 0.7407118335221032 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Prover Message (2nd round)",
            "value": 9752.942069446312,
            "unit": "us/iter",
            "extra": "iterations: 72\ncpu: 9752.183569444443 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Final Verification",
            "value": 9248.674079999546,
            "unit": "us/iter",
            "extra": "iterations: 75\ncpu: 9247.675453333291 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 1st Message (1st round)",
            "value": 33318.083142852454,
            "unit": "us/iter",
            "extra": "iterations: 21\ncpu: 33315.287190476505 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Verifier's Challenge (2nd round)",
            "value": 2.9549904317509936,
            "unit": "us/iter",
            "extra": "iterations: 237243\ncpu: 2.95486902458664 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 2nd Message (3rd round)",
            "value": 351.64039448621526,
            "unit": "us/iter",
            "extra": "iterations: 1995\ncpu: 351.60848421052674 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Final Verification (3rd round)",
            "value": 35212.80260001731,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35210.100049999935 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 1st Message (1st round)",
            "value": 35577.05615000941,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35574.92749999937 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Verifier's Challenge (2nd round)",
            "value": 0.8335691605382217,
            "unit": "us/iter",
            "extra": "iterations: 829794\ncpu: 0.8335167113765533 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 2nd Message (3rd round)",
            "value": 706.822518630412,
            "unit": "us/iter",
            "extra": "iterations: 993\ncpu: 706.7542145015183 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two Paillier Equal / Final Verification (3rd round)",
            "value": 73602.96622219821,
            "unit": "us/iter",
            "extra": "iterations: 9\ncpu: 73598.07166666756 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 1st Message (1st round)",
            "value": 129846.75020006762,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129836.76839999703 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 1st Message (1st round)",
            "value": 129537.8337999864,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129520.081000004 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.7509696515733675,
            "unit": "us/iter",
            "extra": "iterations: 933195\ncpu: 0.7509101066764998 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.7500649908080887,
            "unit": "us/iter",
            "extra": "iterations: 932855\ncpu: 0.7500127468899228 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 6.326466406616549,
            "unit": "us/iter",
            "extra": "iterations: 121259\ncpu: 6.326176094145517 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 6.519843662060604,
            "unit": "us/iter",
            "extra": "iterations: 125798\ncpu: 6.519280552950082 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Final Verification (3rd round)",
            "value": 5.016402092676004,
            "unit": "us/iter",
            "extra": "iterations: 129786\ncpu: 5.016111884178441 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Final Verification (3rd round)",
            "value": 6.862324696325483,
            "unit": "us/iter",
            "extra": "iterations: 125545\ncpu: 6.861449990043494 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 1st Message (1st round)",
            "value": 20453.107470583585,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20451.815882353232 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 1st Message (1st round)",
            "value": 20441.370441176332,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20439.123970588218 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.9016695715647048,
            "unit": "us/iter",
            "extra": "iterations: 779488\ncpu: 0.9016365024221138 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.9215011328786923,
            "unit": "us/iter",
            "extra": "iterations: 778989\ncpu: 0.9214006154130799 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 353.14850352818894,
            "unit": "us/iter",
            "extra": "iterations: 1984\ncpu: 353.1050670362803 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 354.4714563352733,
            "unit": "us/iter",
            "extra": "iterations: 1981\ncpu: 354.45281827360594 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Final Verification (3rd round)",
            "value": 43797.61387500025,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43794.17218749993 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Final Verification (3rd round)",
            "value": 43819.03525001007,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43815.18256250061 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Prover",
            "value": 989.2742503519715,
            "unit": "us/iter",
            "extra": "iterations: 711\ncpu: 989.2248410689306 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Prover",
            "value": 910.2366181816086,
            "unit": "us/iter",
            "extra": "iterations: 770\ncpu: 910.2021584415335 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Verifier",
            "value": 767.877188389725,
            "unit": "us/iter",
            "extra": "iterations: 913\ncpu: 767.828633077725 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Verifier",
            "value": 4605.05436841997,
            "unit": "us/iter",
            "extra": "iterations: 152\ncpu: 4604.753559210382 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Prover",
            "value": 77.4712159391634,
            "unit": "us/iter",
            "extra": "iterations: 9072\ncpu: 77.46596274251011 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Verifier",
            "value": 139.25942575219779,
            "unit": "us/iter",
            "extra": "iterations: 5118\ncpu: 139.2523804220394 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Prover/3",
            "value": 2696.53106614815,
            "unit": "us/iter",
            "extra": "iterations: 257\ncpu: 2696.1451712062844 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Verify/3",
            "value": 1076.5568767334687,
            "unit": "us/iter",
            "extra": "iterations: 649\ncpu: 1076.5175624037238 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Prover",
            "value": 77.99710904846778,
            "unit": "us/iter",
            "extra": "iterations: 8996\ncpu: 77.99136060471446 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Verifier",
            "value": 143.81330233031494,
            "unit": "us/iter",
            "extra": "iterations: 4892\ncpu: 143.8082992641011 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Prover/3",
            "value": 256.39001502943563,
            "unit": "us/iter",
            "extra": "iterations: 2728\ncpu: 256.365736436958 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Verify/3",
            "value": 367.86847429187037,
            "unit": "us/iter",
            "extra": "iterations: 1906\ncpu: 367.8363656873061 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Prover/3",
            "value": 4192.951321213265,
            "unit": "us/iter",
            "extra": "iterations: 165\ncpu: 4192.622775757847 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Verify/3",
            "value": 999.1790300000503,
            "unit": "us/iter",
            "extra": "iterations: 700\ncpu: 999.1239285714723 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Prover",
            "value": 23427.340966660875,
            "unit": "us/iter",
            "extra": "iterations: 30\ncpu: 23425.444266666775 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Verifier",
            "value": 22557.09983870885,
            "unit": "us/iter",
            "extra": "iterations: 31\ncpu: 22556.392419353928 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Prover",
            "value": 92440.06599999467,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 92433.61374999637 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Verifier",
            "value": 87730.70712499021,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 87728.71962500516 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Prover",
            "value": 90748.291875002,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 90742.5077500008 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Verifier",
            "value": 183614.19500001828,
            "unit": "us/iter",
            "extra": "iterations: 4\ncpu: 183603.03775000374 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Prover",
            "value": 331926.46049997164,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 331908.52449999395 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Verifier",
            "value": 17480.862475008507,
            "unit": "us/iter",
            "extra": "iterations: 40\ncpu: 17479.1122000002 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Prover",
            "value": 48060.433600009375,
            "unit": "us/iter",
            "extra": "iterations: 15\ncpu: 48058.32399999872 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Verifier",
            "value": 98866.70314283817,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 98859.32757142816 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Prover",
            "value": 382211.44600015576,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 382201.0889999774 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Verifier",
            "value": 116919.39016661006,
            "unit": "us/iter",
            "extra": "iterations: 6\ncpu: 116912.66116667028 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Prover",
            "value": 387104.70700016234,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 387075.92150001344 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Verifier",
            "value": 126894.96640005018,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 126887.43659999773 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Prover",
            "value": 296318.20199983846,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 296307.175499976 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Verifier",
            "value": 298863.86899988796,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 298849.254500027 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "085d71b9928dc011a806af2cecb42af946374098",
          "message": "build: Improve OpenSSL build process and fix on macOS (#52)\n\nThis commit introduces several improvements to the build system, focusing on making the custom OpenSSL dependency easier to manage and fixing critical ordering issues in CMake.\n\nOpenSSL Build Improvements\nThe previous process for building the required custom OpenSSL was manual and error-prone. This change streamlines the entire experience.\n\nNew make openssl Target\nA new, platform-aware make openssl target has been added. It automatically detects the user's OS (macOS/Linux) and architecture (x86_64/ARM64) and runs the appropriate build script. This simplifies the setup instructions to a single command.\n\nCustomizable Install Path: The OpenSSL installation path was previously hardcoded. Now, users can specify a custom location by setting the CBMPC_OPENSSL_ROOT environment or CMake variable. This is crucial for environments where the user lacks write permissions to /usr/local/opt.\n\nCMake Fixes\nSeveral logical errors in the CMake configuration have been corrected to ensure a more reliable and deterministic build.",
          "timestamp": "2025-10-08T08:01:18-07:00",
          "tree_id": "d52df688bc731cf341ddc8532c399f476036fa54",
          "url": "https://github.com/coinbase/cb-mpc/commit/085d71b9928dc011a806af2cecb42af946374098"
        },
        "date": 1759938076582,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "ZK/Batch-DL/Prover/3/1",
            "value": 1353.7963583813666,
            "unit": "us/iter",
            "extra": "iterations: 519\ncpu: 1353.610421965318 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/1",
            "value": 1239.8679588551724,
            "unit": "us/iter",
            "extra": "iterations: 559\ncpu: 1239.7601198568873 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Prover",
            "value": 1701.6671019421851,
            "unit": "us/iter",
            "extra": "iterations: 412\ncpu: 1701.5597572815534 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Prover",
            "value": 1603.0861499995228,
            "unit": "us/iter",
            "extra": "iterations: 440\ncpu: 1602.9833727272726 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Prover",
            "value": 3981.797337143819,
            "unit": "us/iter",
            "extra": "iterations: 175\ncpu: 3981.348777142859 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Prover",
            "value": 3856.549245811343,
            "unit": "us/iter",
            "extra": "iterations: 179\ncpu: 3856.2005921787686 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Prover",
            "value": 11739.300448276668,
            "unit": "us/iter",
            "extra": "iterations: 58\ncpu: 11738.576551724129 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Prover",
            "value": 12018.285607147001,
            "unit": "us/iter",
            "extra": "iterations: 56\ncpu: 12016.987321428562 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/1",
            "value": 1462.4182898546633,
            "unit": "us/iter",
            "extra": "iterations: 483\ncpu: 1462.3015134575587 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/1",
            "value": 6193.178964602226,
            "unit": "us/iter",
            "extra": "iterations: 113\ncpu: 6192.385486725658 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Verifier",
            "value": 2233.7010705116018,
            "unit": "us/iter",
            "extra": "iterations: 312\ncpu: 2233.589496794875 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Verifier",
            "value": 14731.83825531491,
            "unit": "us/iter",
            "extra": "iterations: 47\ncpu: 14730.568468085106 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Verifier",
            "value": 5659.331379030346,
            "unit": "us/iter",
            "extra": "iterations: 124\ncpu: 5659.007637096771 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Verifier",
            "value": 48715.52599999112,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 48712.883714285716 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Verifier",
            "value": 28184.798680013046,
            "unit": "us/iter",
            "extra": "iterations: 25\ncpu: 28182.184799999985 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Verifier",
            "value": 273294.81766673777,
            "unit": "us/iter",
            "extra": "iterations: 3\ncpu: 273261.8036666666 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Verifier's Challenge (1st round)",
            "value": 0.7399759650564075,
            "unit": "us/iter",
            "extra": "iterations: 953986\ncpu: 0.7399039744818041 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Prover Message (2nd round)",
            "value": 9748.000999997606,
            "unit": "us/iter",
            "extra": "iterations: 72\ncpu: 9746.978027777686 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Final Verification",
            "value": 9246.672666668019,
            "unit": "us/iter",
            "extra": "iterations: 75\ncpu: 9245.610960000002 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 1st Message (1st round)",
            "value": 33312.72157144364,
            "unit": "us/iter",
            "extra": "iterations: 21\ncpu: 33308.58638095232 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Verifier's Challenge (2nd round)",
            "value": 2.945942275264741,
            "unit": "us/iter",
            "extra": "iterations: 237212\ncpu: 2.945673992040904 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 2nd Message (3rd round)",
            "value": 351.8415183139696,
            "unit": "us/iter",
            "extra": "iterations: 1993\ncpu: 351.80931811339974 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Final Verification (3rd round)",
            "value": 35156.79369997997,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35154.80699999998 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 1st Message (1st round)",
            "value": 35570.26815001336,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35568.230150000345 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Verifier's Challenge (2nd round)",
            "value": 0.8444132614998213,
            "unit": "us/iter",
            "extra": "iterations: 828398\ncpu: 0.8443182190203319 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 2nd Message (3rd round)",
            "value": 704.5884179257332,
            "unit": "us/iter",
            "extra": "iterations: 993\ncpu: 704.5158207451998 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two Paillier Equal / Final Verification (3rd round)",
            "value": 73611.84988889768,
            "unit": "us/iter",
            "extra": "iterations: 9\ncpu: 73608.68944444595 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 1st Message (1st round)",
            "value": 129529.41940002349,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129518.80600000436 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 1st Message (1st round)",
            "value": 129517.39999998608,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129511.95759999906 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.7441499245592749,
            "unit": "us/iter",
            "extra": "iterations: 933843\ncpu: 0.7441056922844567 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.7460509660314238,
            "unit": "us/iter",
            "extra": "iterations: 941843\ncpu: 0.7459861218907815 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 5.222993015762633,
            "unit": "us/iter",
            "extra": "iterations: 125998\ncpu: 5.222489595073098 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 5.211905734800525,
            "unit": "us/iter",
            "extra": "iterations: 114337\ncpu: 5.211621985883766 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Final Verification (3rd round)",
            "value": 5.016551739009784,
            "unit": "us/iter",
            "extra": "iterations: 102012\ncpu: 5.016296308277403 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Final Verification (3rd round)",
            "value": 5.004433047152304,
            "unit": "us/iter",
            "extra": "iterations: 124864\ncpu: 5.003949809392685 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 1st Message (1st round)",
            "value": 20444.572705882285,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20443.427411764755 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 1st Message (1st round)",
            "value": 20436.36961764819,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20435.18664705894 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.9079110846381683,
            "unit": "us/iter",
            "extra": "iterations: 780675\ncpu: 0.9078676978255841 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.9603766502527732,
            "unit": "us/iter",
            "extra": "iterations: 765413\ncpu: 0.9603087627202522 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 354.02563512130644,
            "unit": "us/iter",
            "extra": "iterations: 1976\ncpu: 354.00219028340456 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 353.9169089988891,
            "unit": "us/iter",
            "extra": "iterations: 1978\ncpu: 353.8695050556112 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Final Verification (3rd round)",
            "value": 43993.19774998389,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43986.73062499725 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Final Verification (3rd round)",
            "value": 43910.225125017634,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43907.971250000344 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Prover",
            "value": 986.6130466765305,
            "unit": "us/iter",
            "extra": "iterations: 707\ncpu: 986.5337100424183 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Prover",
            "value": 956.8650205388699,
            "unit": "us/iter",
            "extra": "iterations: 779\ncpu: 956.7689448010448 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Verifier",
            "value": 769.5545258528348,
            "unit": "us/iter",
            "extra": "iterations: 909\ncpu: 769.4334576457874 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Verifier",
            "value": 4486.627294871444,
            "unit": "us/iter",
            "extra": "iterations: 156\ncpu: 4486.213743589564 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Prover",
            "value": 77.7885619121335,
            "unit": "us/iter",
            "extra": "iterations: 9037\ncpu: 77.7798536018607 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Verifier",
            "value": 138.13876956781024,
            "unit": "us/iter",
            "extra": "iterations: 5021\ncpu: 138.12846444931145 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Prover/3",
            "value": 2705.272527131202,
            "unit": "us/iter",
            "extra": "iterations: 258\ncpu: 2705.0272170540684 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Verify/3",
            "value": 1075.7417987324213,
            "unit": "us/iter",
            "extra": "iterations: 631\ncpu: 1075.5967543580823 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Prover",
            "value": 78.1138219468595,
            "unit": "us/iter",
            "extra": "iterations: 8958\ncpu: 78.10849933020778 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Verifier",
            "value": 142.67859596992287,
            "unit": "us/iter",
            "extra": "iterations: 4913\ncpu: 142.66147303073535 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Prover/3",
            "value": 260.3885515351022,
            "unit": "us/iter",
            "extra": "iterations: 2736\ncpu: 260.36297039473567 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Verify/3",
            "value": 370.8622349555096,
            "unit": "us/iter",
            "extra": "iterations: 1911\ncpu: 370.8279863945592 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Prover/3",
            "value": 4214.593351513362,
            "unit": "us/iter",
            "extra": "iterations: 165\ncpu: 4213.947848484856 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Verify/3",
            "value": 999.0546771431192,
            "unit": "us/iter",
            "extra": "iterations: 700\ncpu: 998.9145257143101 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Prover",
            "value": 23518.188733335894,
            "unit": "us/iter",
            "extra": "iterations: 30\ncpu: 23515.06509999884 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Verifier",
            "value": 22570.913741938348,
            "unit": "us/iter",
            "extra": "iterations: 31\ncpu: 22568.63961290382 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Prover",
            "value": 92434.16912499924,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 92429.42174999768 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Verifier",
            "value": 87835.122125,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 87823.90412500262 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Prover",
            "value": 90726.16250000465,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 90713.95237499758 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Verifier",
            "value": 183841.53399995284,
            "unit": "us/iter",
            "extra": "iterations: 4\ncpu: 183826.66299999075 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Prover",
            "value": 331614.66900014604,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 331577.1080000047 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Verifier",
            "value": 17482.665850002377,
            "unit": "us/iter",
            "extra": "iterations: 40\ncpu: 17480.08019999929 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Prover",
            "value": 48166.865799976214,
            "unit": "us/iter",
            "extra": "iterations: 15\ncpu: 48164.07353333337 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Verifier",
            "value": 99029.46857144863,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 99022.0650000018 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Prover",
            "value": 382076.4854999652,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 382055.9254999978 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Verifier",
            "value": 117413.52083337611,
            "unit": "us/iter",
            "extra": "iterations: 6\ncpu: 117307.60499999822 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Prover",
            "value": 387592.8439999825,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 387552.7545000068 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Verifier",
            "value": 127201.15319998513,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 127187.19279999958 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Prover",
            "value": 296853.85500010854,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 296836.22499999276 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Verifier",
            "value": 297994.62100004347,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 297968.00699998013 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "ec9a818f748e59ecec24676272d62e7e9068f6f1",
          "message": "fix: Prevent buffer overflow in converter (#54)\n\nAdd a bounds check in bits_t::convert to ensure there is sufficient data in the source buffer before deserializing. Also, add an assertion in converter_t::forward to protect against integer overflow on the offset.\n\nRemove the bufs128_t and bufs128_ref_t classes along with their associated functions, as they were not being used anywhere in the codebase.",
          "timestamp": "2025-10-09T11:02:02-07:00",
          "tree_id": "2157f87e79b77ff4fb1a01b419249988bf3e26e2",
          "url": "https://github.com/coinbase/cb-mpc/commit/ec9a818f748e59ecec24676272d62e7e9068f6f1"
        },
        "date": 1760035278713,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "ZK/Batch-DL/Prover/3/1",
            "value": 1349.3987423076376,
            "unit": "us/iter",
            "extra": "iterations: 520\ncpu: 1349.2836788461539 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/1",
            "value": 1239.8996872728028,
            "unit": "us/iter",
            "extra": "iterations: 550\ncpu: 1239.8327581818178 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Prover",
            "value": 1701.9799491524698,
            "unit": "us/iter",
            "extra": "iterations: 413\ncpu: 1701.7733002421305 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Prover",
            "value": 1600.5614269406412,
            "unit": "us/iter",
            "extra": "iterations: 438\ncpu: 1600.4590342465754 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Prover",
            "value": 3958.470651428537,
            "unit": "us/iter",
            "extra": "iterations: 175\ncpu: 3958.012811428571 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Prover",
            "value": 3881.3784166664946,
            "unit": "us/iter",
            "extra": "iterations: 180\ncpu: 3881.0769722222194 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Prover",
            "value": 12133.564500000548,
            "unit": "us/iter",
            "extra": "iterations: 58\ncpu: 12131.910155172416 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Prover",
            "value": 12050.212799999827,
            "unit": "us/iter",
            "extra": "iterations: 60\ncpu: 12049.473516666669 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/1",
            "value": 1484.261663157859,
            "unit": "us/iter",
            "extra": "iterations: 475\ncpu: 1484.1901052631588 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/1",
            "value": 6173.145097343764,
            "unit": "us/iter",
            "extra": "iterations: 113\ncpu: 6172.781867256639 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Verifier",
            "value": 2262.0234158730163,
            "unit": "us/iter",
            "extra": "iterations: 315\ncpu: 2261.879892063494 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Verifier",
            "value": 14744.446500003505,
            "unit": "us/iter",
            "extra": "iterations: 48\ncpu: 14743.078041666673 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Verifier",
            "value": 5399.190104838219,
            "unit": "us/iter",
            "extra": "iterations: 124\ncpu: 5398.844403225808 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Verifier",
            "value": 48483.25214285458,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 48480.362642857086 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Verifier",
            "value": 28269.416960001763,
            "unit": "us/iter",
            "extra": "iterations: 25\ncpu: 28266.951279999972 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Verifier",
            "value": 271790.2103333169,
            "unit": "us/iter",
            "extra": "iterations: 3\ncpu: 271758.277 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Verifier's Challenge (1st round)",
            "value": 0.7321841552100326,
            "unit": "us/iter",
            "extra": "iterations: 948135\ncpu: 0.732123260928034 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Prover Message (2nd round)",
            "value": 9740.10154166649,
            "unit": "us/iter",
            "extra": "iterations: 72\ncpu: 9739.35486111108 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Final Verification",
            "value": 9248.115480001312,
            "unit": "us/iter",
            "extra": "iterations: 75\ncpu: 9247.6460266667 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 1st Message (1st round)",
            "value": 33299.98880952276,
            "unit": "us/iter",
            "extra": "iterations: 21\ncpu: 33297.12814285692 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Verifier's Challenge (2nd round)",
            "value": 2.956619524920748,
            "unit": "us/iter",
            "extra": "iterations: 235750\ncpu: 2.9563294931070936 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 2nd Message (3rd round)",
            "value": 352.0242947686453,
            "unit": "us/iter",
            "extra": "iterations: 1988\ncpu: 351.9836317907412 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Final Verification (3rd round)",
            "value": 35134.869000000894,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35132.399100000104 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 1st Message (1st round)",
            "value": 35681.17240000675,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35677.75295000004 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Verifier's Challenge (2nd round)",
            "value": 0.8314375699559041,
            "unit": "us/iter",
            "extra": "iterations: 843392\ncpu: 0.8313635592938847 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 2nd Message (3rd round)",
            "value": 703.6830861676044,
            "unit": "us/iter",
            "extra": "iterations: 1323\ncpu: 703.6349992441377 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two Paillier Equal / Final Verification (3rd round)",
            "value": 73684.09522223374,
            "unit": "us/iter",
            "extra": "iterations: 9\ncpu: 73674.21611111051 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 1st Message (1st round)",
            "value": 129393.35219998611,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129381.37379999828 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 1st Message (1st round)",
            "value": 129569.70599998385,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129565.78099999661 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.7421066108975979,
            "unit": "us/iter",
            "extra": "iterations: 941264\ncpu: 0.7420785295092591 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.7435636358429533,
            "unit": "us/iter",
            "extra": "iterations: 942582\ncpu: 0.7434908220186646 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 5.980313568616255,
            "unit": "us/iter",
            "extra": "iterations: 95743\ncpu: 5.979852292073527 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 4.695956593981651,
            "unit": "us/iter",
            "extra": "iterations: 112542\ncpu: 4.695193296724887 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Final Verification (3rd round)",
            "value": 5.745739282039034,
            "unit": "us/iter",
            "extra": "iterations: 106970\ncpu: 5.745544077778704 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Final Verification (3rd round)",
            "value": 5.441919268790812,
            "unit": "us/iter",
            "extra": "iterations: 116027\ncpu: 5.441509062545853 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 1st Message (1st round)",
            "value": 20458.858176465692,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20457.528235293492 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 1st Message (1st round)",
            "value": 20469.32567647191,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20468.207941176533 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.9060912002123832,
            "unit": "us/iter",
            "extra": "iterations: 776621\ncpu: 0.9060442133292766 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.8990408456858432,
            "unit": "us/iter",
            "extra": "iterations: 777830\ncpu: 0.898968658961472 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 353.4832065656012,
            "unit": "us/iter",
            "extra": "iterations: 1980\ncpu: 353.4593838383724 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 353.1658933804556,
            "unit": "us/iter",
            "extra": "iterations: 1979\ncpu: 353.1403900959928 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Final Verification (3rd round)",
            "value": 43815.224000013586,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43813.38325000073 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Final Verification (3rd round)",
            "value": 43836.01918749491,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43832.242937497766 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Prover",
            "value": 986.774180536228,
            "unit": "us/iter",
            "extra": "iterations: 709\ncpu: 986.7101424541801 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Prover",
            "value": 911.4658324608686,
            "unit": "us/iter",
            "extra": "iterations: 764\ncpu: 911.4155719895841 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Verifier",
            "value": 771.5344989014989,
            "unit": "us/iter",
            "extra": "iterations: 910\ncpu: 771.4783923076468 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Verifier",
            "value": 4469.979929488032,
            "unit": "us/iter",
            "extra": "iterations: 156\ncpu: 4469.793967948532 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Prover",
            "value": 77.58195884501788,
            "unit": "us/iter",
            "extra": "iterations: 9039\ncpu: 77.57683604380915 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Verifier",
            "value": 139.63522488607128,
            "unit": "us/iter",
            "extra": "iterations: 5047\ncpu: 139.62035763819952 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Prover/3",
            "value": 2742.9055039061013,
            "unit": "us/iter",
            "extra": "iterations: 256\ncpu: 2742.6829218748503 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Verify/3",
            "value": 1075.9733297383636,
            "unit": "us/iter",
            "extra": "iterations: 649\ncpu: 1075.848799691798 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Prover",
            "value": 78.15049815869385,
            "unit": "us/iter",
            "extra": "iterations: 8961\ncpu: 78.14269367258233 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Verifier",
            "value": 140.96079846675195,
            "unit": "us/iter",
            "extra": "iterations: 4957\ncpu: 140.9485928989283 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Prover/3",
            "value": 256.15283841356376,
            "unit": "us/iter",
            "extra": "iterations: 2723\ncpu: 256.12856628719567 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Verify/3",
            "value": 370.3769192777468,
            "unit": "us/iter",
            "extra": "iterations: 1883\ncpu: 370.3456128518403 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Prover/3",
            "value": 4252.464182927059,
            "unit": "us/iter",
            "extra": "iterations: 164\ncpu: 4252.072140244037 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Verify/3",
            "value": 1002.6508242858005,
            "unit": "us/iter",
            "extra": "iterations: 700\ncpu: 1002.5780257142611 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Prover",
            "value": 23435.764166667166,
            "unit": "us/iter",
            "extra": "iterations: 30\ncpu: 23434.455733333456 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Verifier",
            "value": 22568.48996774997,
            "unit": "us/iter",
            "extra": "iterations: 31\ncpu: 22567.142225807034 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Prover",
            "value": 92436.17800001403,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 92427.76125000062 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Verifier",
            "value": 87825.09962497898,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 87820.58937499925 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Prover",
            "value": 90706.7159999997,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 90699.36774999832 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Verifier",
            "value": 183732.27475001386,
            "unit": "us/iter",
            "extra": "iterations: 4\ncpu: 183717.5975000065 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Prover",
            "value": 331685.0890000751,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 331673.92450002354 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Verifier",
            "value": 17476.485274994502,
            "unit": "us/iter",
            "extra": "iterations: 40\ncpu: 17475.26747499961 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Prover",
            "value": 48153.10813331356,
            "unit": "us/iter",
            "extra": "iterations: 15\ncpu: 48149.02559999913 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Verifier",
            "value": 99078.4297142844,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 99072.76542856738 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Prover",
            "value": 382241.9569999056,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 382225.00250000734 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Verifier",
            "value": 117247.58666665973,
            "unit": "us/iter",
            "extra": "iterations: 6\ncpu: 117240.44533333236 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Prover",
            "value": 386849.71900011075,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 386831.6024999956 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Verifier",
            "value": 127029.16179996464,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 127022.832199998 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Prover",
            "value": 296806.1725000553,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 296782.49649998633 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Verifier",
            "value": 298285.28100006224,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 298272.52699999465 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "231b02ec9224a5400473f344c31302caab741e7d",
          "message": "refactor: Consolidate threshold DKG logic into key_share_mp_t (#55)\n\nThe static methods for threshold distributed key generation (DKG) and key refresh have been moved from the `dkg_mp_threshold_t` struct to become static methods of `key_share_mp_t`.\n\nThis refactoring simplifies the code by removing the unnecessary `dkg_mp_threshold_t` struct and centralizes key generation logic within the key share structure itself.\n\nThe functions were renamed to `threshold_dkg` and `threshold_refresh` for clarity. All call sites and protocol wrappers (`ecdsa_mp`, `schnorr_mp`) have been updated to use the new API.",
          "timestamp": "2025-10-18T22:50:15-07:00",
          "tree_id": "820469edfdd766d9cc097b65b5b474fb780e0507",
          "url": "https://github.com/coinbase/cb-mpc/commit/231b02ec9224a5400473f344c31302caab741e7d"
        },
        "date": 1760855393274,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "ZK/Batch-DL/Prover/3/1",
            "value": 1324.2479319468946,
            "unit": "us/iter",
            "extra": "iterations: 529\ncpu: 1324.1884688090738 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/1",
            "value": 1215.4575549737092,
            "unit": "us/iter",
            "extra": "iterations: 573\ncpu: 1215.4533996509597 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Prover",
            "value": 1673.5916004785413,
            "unit": "us/iter",
            "extra": "iterations: 418\ncpu: 1673.517653110048 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Prover",
            "value": 1566.541879819006,
            "unit": "us/iter",
            "extra": "iterations: 441\ncpu: 1566.412548752834 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Prover",
            "value": 3933.1428222213717,
            "unit": "us/iter",
            "extra": "iterations: 180\ncpu: 3933.0314833333314 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Prover",
            "value": 3853.8971693998947,
            "unit": "us/iter",
            "extra": "iterations: 183\ncpu: 3853.889535519122 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Prover",
            "value": 11736.37966101878,
            "unit": "us/iter",
            "extra": "iterations: 59\ncpu: 11735.659033898304 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Prover",
            "value": 11430.488200001795,
            "unit": "us/iter",
            "extra": "iterations: 60\ncpu: 11429.754416666663 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/1",
            "value": 1467.3205124997442,
            "unit": "us/iter",
            "extra": "iterations: 480\ncpu: 1467.261218749999 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/1",
            "value": 6218.269419643921,
            "unit": "us/iter",
            "extra": "iterations: 112\ncpu: 6218.058803571434 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / secp256k1 / Verifier",
            "value": 2230.521064102475,
            "unit": "us/iter",
            "extra": "iterations: 312\ncpu: 2230.4862083333323 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 4) / Ed25519 / Verifier",
            "value": 14765.183510641518,
            "unit": "us/iter",
            "extra": "iterations: 47\ncpu: 14764.465319148934 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / secp256k1 / Verifier",
            "value": 5623.243309523715,
            "unit": "us/iter",
            "extra": "iterations: 126\ncpu: 5623.183095238108 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 16) / Ed25519 / Verifier",
            "value": 48780.715071432365,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 48777.865928571424 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / secp256k1 / Verifier",
            "value": 28439.655920001314,
            "unit": "us/iter",
            "extra": "iterations: 25\ncpu: 28438.986560000005 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / Batch-DL (n = 64) / Ed25519 / Verifier",
            "value": 273165.05633333087,
            "unit": "us/iter",
            "extra": "iterations: 3\ncpu: 273144.2413333334 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Verifier's Challenge (1st round)",
            "value": 0.5278732911082135,
            "unit": "us/iter",
            "extra": "iterations: 1327176\ncpu: 0.5278748508110497 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Prover Message (2nd round)",
            "value": 9734.031402779743,
            "unit": "us/iter",
            "extra": "iterations: 72\ncpu: 9733.278861111157 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Valid-Paillier / Final Verification",
            "value": 9235.26296000091,
            "unit": "us/iter",
            "extra": "iterations: 75\ncpu: 9234.694079999977 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 1st Message (1st round)",
            "value": 33374.9936666685,
            "unit": "us/iter",
            "extra": "iterations: 21\ncpu: 33373.405047619206 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Verifier's Challenge (2nd round)",
            "value": 2.1220516693953315,
            "unit": "us/iter",
            "extra": "iterations: 330389\ncpu: 2.1219816912790734 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Prover's 2nd Message (3rd round)",
            "value": 351.50690576445317,
            "unit": "us/iter",
            "extra": "iterations: 1995\ncpu: 351.4989368421042 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Zero / Final Verification (3rd round)",
            "value": 35181.36625000352,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35180.77579999996 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 1st Message (1st round)",
            "value": 35575.16980001765,
            "unit": "us/iter",
            "extra": "iterations: 20\ncpu: 35573.756349999996 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Verifier's Challenge (2nd round)",
            "value": 0.5986865258304463,
            "unit": "us/iter",
            "extra": "iterations: 1169586\ncpu: 0.5986667872221579 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two-Paillier-Equal / Prover's 2nd Message (3rd round)",
            "value": 704.0374808855108,
            "unit": "us/iter",
            "extra": "iterations: 994\ncpu: 703.9963903420396 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Two Paillier Equal / Final Verification (3rd round)",
            "value": 73506.88888886402,
            "unit": "us/iter",
            "extra": "iterations: 9\ncpu: 73501.6181111108 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 1st Message (1st round)",
            "value": 129344.57280007335,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129341.50479999857 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 1st Message (1st round)",
            "value": 129186.29619998683,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 129184.23380000377 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.5359470812975728,
            "unit": "us/iter",
            "extra": "iterations: 1299276\ncpu: 0.5359039511235401 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.536476434964959,
            "unit": "us/iter",
            "extra": "iterations: 1303011\ncpu: 0.5364598702543703 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 7.620042226562137,
            "unit": "us/iter",
            "extra": "iterations: 129042\ncpu: 7.619744687776172 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 5.5312211747165545,
            "unit": "us/iter",
            "extra": "iterations: 108724\ncpu: 5.530744389463283 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / secp256k1 / Final Verification (3rd round)",
            "value": 3.9205107168017346,
            "unit": "us/iter",
            "extra": "iterations: 137681\ncpu: 3.920436174926104 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Range-Pedersen / Ed25519 / Final Verification (3rd round)",
            "value": 5.998901621924437,
            "unit": "us/iter",
            "extra": "iterations: 127996\ncpu: 5.998632676021163 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 1st Message (1st round)",
            "value": 20437.16311765606,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20435.957382352568 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 1st Message (1st round)",
            "value": 20490.783117646275,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20489.94520588277 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Verifier's Challenge (2nd round)",
            "value": 0.6714567051004151,
            "unit": "us/iter",
            "extra": "iterations: 1044742\ncpu: 0.6714155466134072 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Verifier's Challenge (2nd round)",
            "value": 0.6723444342073359,
            "unit": "us/iter",
            "extra": "iterations: 1046284\ncpu: 0.6723128376234779 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Prover's 2nd Message (3rd round)",
            "value": 353.51914444443605,
            "unit": "us/iter",
            "extra": "iterations: 1980\ncpu: 353.51389292929866 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Prover's 2nd Message (3rd round)",
            "value": 353.31692171728884,
            "unit": "us/iter",
            "extra": "iterations: 1980\ncpu: 353.2974191919245 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / secp256k1 / Final Verification (3rd round)",
            "value": 43719.708374993614,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43718.889312497566 us\nthreads: 1"
          },
          {
            "name": "ZK (Interactive) / Paillier-Pedersen-Equal / Ed25519 / Final Verification (3rd round)",
            "value": 43717.89031250728,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43716.77662500062 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Prover",
            "value": 971.7201824509565,
            "unit": "us/iter",
            "extra": "iterations: 718\ncpu: 971.6954192200651 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Prover",
            "value": 895.4132155614453,
            "unit": "us/iter",
            "extra": "iterations: 784\ncpu: 895.3788443877105 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / secp256k1 / Verifier",
            "value": 756.9241137594283,
            "unit": "us/iter",
            "extra": "iterations: 923\ncpu: 756.8881939328448 us\nthreads: 1"
          },
          {
            "name": "UC-ZK / DL / Ed25519 / Verifier",
            "value": 4503.275509676079,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4503.171058064665 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Prover",
            "value": 77.15395935044673,
            "unit": "us/iter",
            "extra": "iterations: 9053\ncpu: 77.1499787915618 us\nthreads: 1"
          },
          {
            "name": "ZK / DH / secp256k1 / Verifier",
            "value": 139.50540928767535,
            "unit": "us/iter",
            "extra": "iterations: 5082\ncpu: 139.49998681621668 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Prover/3",
            "value": 2704.356574711612,
            "unit": "us/iter",
            "extra": "iterations: 261\ncpu: 2704.236697318056 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Verify/3",
            "value": 1069.0713734754254,
            "unit": "us/iter",
            "extra": "iterations: 656\ncpu: 1069.065262195098 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Prover",
            "value": 77.61603916562117,
            "unit": "us/iter",
            "extra": "iterations: 9013\ncpu: 77.6138474425868 us\nthreads: 1"
          },
          {
            "name": "ZK / ElGamal-PubShare-Equal / secp256k1 / Verifier",
            "value": 140.50930567412303,
            "unit": "us/iter",
            "extra": "iterations: 4917\ncpu: 140.5043603823461 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Prover/3",
            "value": 255.06932630052736,
            "unit": "us/iter",
            "extra": "iterations: 2749\ncpu: 255.0578537650076 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Verify/3",
            "value": 368.0814152946416,
            "unit": "us/iter",
            "extra": "iterations: 1883\ncpu: 368.06849442379547 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Prover/3",
            "value": 4166.270240964504,
            "unit": "us/iter",
            "extra": "iterations: 166\ncpu: 4166.158283132712 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Verify/3",
            "value": 996.7877837833648,
            "unit": "us/iter",
            "extra": "iterations: 703\ncpu: 996.7297795162899 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Prover",
            "value": 23462.07766666642,
            "unit": "us/iter",
            "extra": "iterations: 30\ncpu: 23459.669099999777 us\nthreads: 1"
          },
          {
            "name": "ZK / Valid-Paillier / Verifier",
            "value": 22563.190419356444,
            "unit": "us/iter",
            "extra": "iterations: 31\ncpu: 22562.253967743298 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Prover",
            "value": 92528.72937503298,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 92524.35762500255 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Zero / Verifier",
            "value": 87785.22475000727,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 87783.60937499485 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Prover",
            "value": 90647.39512501773,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 90643.04837500004 us\nthreads: 1"
          },
          {
            "name": "ZK / Two-Paillier-Equal / Verifier",
            "value": 183345.90199992817,
            "unit": "us/iter",
            "extra": "iterations: 4\ncpu: 183334.57675001342 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Prover",
            "value": 331580.96650004154,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 331575.8795000079 us\nthreads: 1"
          },
          {
            "name": "ZK / Range-Pedersen / secp256k1 / Verifier",
            "value": 17456.494900000052,
            "unit": "us/iter",
            "extra": "iterations: 40\ncpu: 17455.78532500076 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Prover",
            "value": 48154.857000008626,
            "unit": "us/iter",
            "extra": "iterations: 15\ncpu: 48153.08186666698 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Pedersen-Equal / secp256k1 / Verifier",
            "value": 98902.77442855222,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 98901.40157143281 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Prover",
            "value": 382286.49100005894,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 382282.15500001283 us\nthreads: 1"
          },
          {
            "name": "ZK / Paillier-Range-Exp-Slack / secp256k1 / Verifier",
            "value": 116890.68466663836,
            "unit": "us/iter",
            "extra": "iterations: 6\ncpu: 116888.81516666546 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Prover",
            "value": 387753.0539998588,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 387738.02500000445 us\nthreads: 1"
          },
          {
            "name": "ZK / PDL / Verifier",
            "value": 127025.22380004665,
            "unit": "us/iter",
            "extra": "iterations: 5\ncpu: 127018.16860000009 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Prover",
            "value": 297582.9350000367,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 297570.50550000486 us\nthreads: 1"
          },
          {
            "name": "ZK / Unknown-Order-DL / Verifier",
            "value": 297299.27749986015,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 297297.48650001397 us\nthreads: 1"
          }
        ]
      },
      {
        "commit": {
          "author": {
            "email": "yihsiuc@pm.me",
            "name": "Yi-Hsiu Chen",
            "username": "hsiuhsiu"
          },
          "committer": {
            "email": "noreply@github.com",
            "name": "GitHub",
            "username": "web-flow"
          },
          "distinct": true,
          "id": "50522e0ded8eefead310ba9c99513bd01751c7d6",
          "message": "fix: Prevent Out-of-Bounds read in PVE (#57)",
          "timestamp": "2025-10-27T08:13:03-07:00",
          "tree_id": "c54d31a69486f2e1673f2fb747f0cf27aaef3e32",
          "url": "https://github.com/coinbase/cb-mpc/commit/50522e0ded8eefead310ba9c99513bd01751c7d6"
        },
        "date": 1761580219523,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "ZK/Batch-DL/Prover/3/1",
            "value": 1254.522899461608,
            "unit": "us/iter",
            "extra": "iterations: 557\ncpu: 1254.4652082585278 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/1",
            "value": 1243.99488256254,
            "unit": "us/iter",
            "extra": "iterations: 562\ncpu: 1243.923240213523 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/3/4",
            "value": 1613.9089280744222,
            "unit": "us/iter",
            "extra": "iterations: 431\ncpu: 1613.8032320185619 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/4",
            "value": 1612.01576223752,
            "unit": "us/iter",
            "extra": "iterations: 429\ncpu: 1611.8065151515152 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/3/16",
            "value": 3809.0060111105436,
            "unit": "us/iter",
            "extra": "iterations: 180\ncpu: 3808.8442777777777 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/16",
            "value": 3916.6031833335383,
            "unit": "us/iter",
            "extra": "iterations: 180\ncpu: 3916.303272222225 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/3/64",
            "value": 12030.160295081096,
            "unit": "us/iter",
            "extra": "iterations: 61\ncpu: 12029.408393442629 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Prover/4/64",
            "value": 12090.821767860203,
            "unit": "us/iter",
            "extra": "iterations: 56\ncpu: 12089.287053571421 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/1",
            "value": 1482.0645075267612,
            "unit": "us/iter",
            "extra": "iterations: 465\ncpu: 1482.0271311827973 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/1",
            "value": 5880.7036722700705,
            "unit": "us/iter",
            "extra": "iterations: 119\ncpu: 5880.453126050424 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/4",
            "value": 2213.04833546285,
            "unit": "us/iter",
            "extra": "iterations: 313\ncpu: 2212.949674121409 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/4",
            "value": 13939.365359997282,
            "unit": "us/iter",
            "extra": "iterations: 50\ncpu: 13938.844100000018 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/16",
            "value": 5471.041789061104,
            "unit": "us/iter",
            "extra": "iterations: 128\ncpu: 5470.6400234375005 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/16",
            "value": 46085.47266666392,
            "unit": "us/iter",
            "extra": "iterations: 15\ncpu: 46081.45319999994 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/3/64",
            "value": 27397.853279999254,
            "unit": "us/iter",
            "extra": "iterations: 25\ncpu: 27395.479639999965 us\nthreads: 1"
          },
          {
            "name": "ZK/Batch-DL/Verify/4/64",
            "value": 258460.97433327488,
            "unit": "us/iter",
            "extra": "iterations: 3\ncpu: 258440.8673333331 us\nthreads: 1"
          },
          {
            "name": "ZK/ValidPaillier-Int/V1",
            "value": 0.43009699264835366,
            "unit": "us/iter",
            "extra": "iterations: 1628742\ncpu: 0.4300504008615214 us\nthreads: 1"
          },
          {
            "name": "ZK/ValidPaillier-Int/P2",
            "value": 8822.976620255264,
            "unit": "us/iter",
            "extra": "iterations: 79\ncpu: 8821.30915189877 us\nthreads: 1"
          },
          {
            "name": "ZK/ValidPaillier-Int/Verify",
            "value": 8395.109867470605,
            "unit": "us/iter",
            "extra": "iterations: 83\ncpu: 8394.50649397593 us\nthreads: 1"
          },
          {
            "name": "ZK/PaillierZero-Int/P1",
            "value": 30859.79352173403,
            "unit": "us/iter",
            "extra": "iterations: 23\ncpu: 30859.165739130698 us\nthreads: 1"
          },
          {
            "name": "ZK/PaillierZero-Int/V2",
            "value": 1.7272156168873318,
            "unit": "us/iter",
            "extra": "iterations: 405228\ncpu: 1.7271440941889464 us\nthreads: 1"
          },
          {
            "name": "ZK/PaillierZero-Int/P3",
            "value": 328.47941525422874,
            "unit": "us/iter",
            "extra": "iterations: 2124\ncpu: 328.4572923728806 us\nthreads: 1"
          },
          {
            "name": "ZK/PaillierZero-Int/Verify",
            "value": 32636.153142859606,
            "unit": "us/iter",
            "extra": "iterations: 21\ncpu: 32633.58133333346 us\nthreads: 1"
          },
          {
            "name": "ZK/TwoPaillierEqual-Int/P1",
            "value": 32211.320272722336,
            "unit": "us/iter",
            "extra": "iterations: 22\ncpu: 32206.226590908864 us\nthreads: 1"
          },
          {
            "name": "ZK/TwoPaillierEqual-Int/V2",
            "value": 0.48043223175073335,
            "unit": "us/iter",
            "extra": "iterations: 1460293\ncpu: 0.4803756554335388 us\nthreads: 1"
          },
          {
            "name": "ZK/TwoPaillierEqual-Int/P3",
            "value": 658.3106829728666,
            "unit": "us/iter",
            "extra": "iterations: 1063\ncpu: 658.2291947318805 us\nthreads: 1"
          },
          {
            "name": "ZK/TwoPaillierEqual-Int/Verify",
            "value": 67098.06269998352,
            "unit": "us/iter",
            "extra": "iterations: 10\ncpu: 67091.92739999991 us\nthreads: 1"
          },
          {
            "name": "ZK/RangePedersenInt/P1/3",
            "value": 116862.45883333868,
            "unit": "us/iter",
            "extra": "iterations: 6\ncpu: 116850.65866666852 us\nthreads: 1"
          },
          {
            "name": "ZK/RangePedersenInt/P1/4",
            "value": 117208.04399999452,
            "unit": "us/iter",
            "extra": "iterations: 6\ncpu: 117197.52633333276 us\nthreads: 1"
          },
          {
            "name": "ZK/RangePedersenInt/V2/3",
            "value": 0.4347719335899209,
            "unit": "us/iter",
            "extra": "iterations: 1602770\ncpu: 0.4347218340747596 us\nthreads: 1"
          },
          {
            "name": "ZK/RangePedersenInt/V2/4",
            "value": 0.435010299533657,
            "unit": "us/iter",
            "extra": "iterations: 1610170\ncpu: 0.4349662818211739 us\nthreads: 1"
          },
          {
            "name": "ZK/RangePedersenInt/P3/3",
            "value": 5.094831605403509,
            "unit": "us/iter",
            "extra": "iterations: 133686\ncpu: 5.094481628592371 us\nthreads: 1"
          },
          {
            "name": "ZK/RangePedersenInt/P3/4",
            "value": 6.239453870000489,
            "unit": "us/iter",
            "extra": "iterations: 100000\ncpu: 6.238662769999905 us\nthreads: 1"
          },
          {
            "name": "ZK/RangePedersenInt/Verify/3",
            "value": 5.817317660000754,
            "unit": "us/iter",
            "extra": "iterations: 100000\ncpu: 5.816640449999966 us\nthreads: 1"
          },
          {
            "name": "ZK/RangePedersenInt/Verify/4",
            "value": 4.442914409414642,
            "unit": "us/iter",
            "extra": "iterations: 127035\ncpu: 4.442454048097007 us\nthreads: 1"
          },
          {
            "name": "ZK/PaillierPedersenEq-Int/P1/3",
            "value": 18974.927729729912,
            "unit": "us/iter",
            "extra": "iterations: 37\ncpu: 18973.427756756602 us\nthreads: 1"
          },
          {
            "name": "ZK/PaillierPedersenEq-Int/P1/4",
            "value": 19000.923783784634,
            "unit": "us/iter",
            "extra": "iterations: 37\ncpu: 18998.947027026785 us\nthreads: 1"
          },
          {
            "name": "ZK/PaillierPedersenEq-Int/V2/3",
            "value": 0.5191728979721753,
            "unit": "us/iter",
            "extra": "iterations: 1348888\ncpu: 0.5191439578378723 us\nthreads: 1"
          },
          {
            "name": "ZK/PaillierPedersenEq-Int/V2/4",
            "value": 0.5196157745189236,
            "unit": "us/iter",
            "extra": "iterations: 1345106\ncpu: 0.519563385339138 us\nthreads: 1"
          },
          {
            "name": "ZK/PaillierPedersenEq-Int/P3/3",
            "value": 330.2572920605193,
            "unit": "us/iter",
            "extra": "iterations: 2116\ncpu: 330.23477362948597 us\nthreads: 1"
          },
          {
            "name": "ZK/PaillierPedersenEq-Int/P3/4",
            "value": 330.5571708352142,
            "unit": "us/iter",
            "extra": "iterations: 2119\ncpu: 330.51635630014295 us\nthreads: 1"
          },
          {
            "name": "ZK/PaillierPedersenEq-Int/Verifier/3",
            "value": 40036.23494118609,
            "unit": "us/iter",
            "extra": "iterations: 17\ncpu: 40031.14276470315 us\nthreads: 1"
          },
          {
            "name": "ZK/PaillierPedersenEq-Int/Verifier/4",
            "value": 40054.75723528542,
            "unit": "us/iter",
            "extra": "iterations: 17\ncpu: 40053.475647057974 us\nthreads: 1"
          },
          {
            "name": "ZK/DL/Prover/3",
            "value": 924.4873771351623,
            "unit": "us/iter",
            "extra": "iterations: 761\ncpu: 924.3818120893852 us\nthreads: 1"
          },
          {
            "name": "ZK/DL/Prover/4",
            "value": 917.7253001310384,
            "unit": "us/iter",
            "extra": "iterations: 763\ncpu: 917.6075570118363 us\nthreads: 1"
          },
          {
            "name": "ZK/DL/Verify/3",
            "value": 781.5865050620882,
            "unit": "us/iter",
            "extra": "iterations: 889\ncpu: 781.5032238469952 us\nthreads: 1"
          },
          {
            "name": "ZK/DL/Verify/4",
            "value": 4261.742274390998,
            "unit": "us/iter",
            "extra": "iterations: 164\ncpu: 4261.315871951311 us\nthreads: 1"
          },
          {
            "name": "ZK/DH/Prove/3",
            "value": 79.1167806889006,
            "unit": "us/iter",
            "extra": "iterations: 8855\ncpu: 79.1085016374922 us\nthreads: 1"
          },
          {
            "name": "ZK/DH/Verify/3",
            "value": 138.73679560351604,
            "unit": "us/iter",
            "extra": "iterations: 5186\ncpu: 138.71696934053205 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Prover/3",
            "value": 2612.083981343451,
            "unit": "us/iter",
            "extra": "iterations: 268\ncpu: 2611.736880597057 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalCom/Verify/3",
            "value": 1104.145321766797,
            "unit": "us/iter",
            "extra": "iterations: 634\ncpu: 1104.0554605678187 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalPubShareEqual/Prover/3",
            "value": 79.8418319798671,
            "unit": "us/iter",
            "extra": "iterations: 8743\ncpu: 79.83739700332009 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalPubShareEqual/Verify/3",
            "value": 142.71001972064457,
            "unit": "us/iter",
            "extra": "iterations: 4868\ncpu: 142.69762612982922 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Prover/3",
            "value": 252.66887059241003,
            "unit": "us/iter",
            "extra": "iterations: 2751\ncpu: 252.644425663399 us\nthreads: 1"
          },
          {
            "name": "ZK/ElGamalComMult/Verify/3",
            "value": 359.77139782717006,
            "unit": "us/iter",
            "extra": "iterations: 1933\ncpu: 359.7237904810976 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Prover/3",
            "value": 4136.010976191344,
            "unit": "us/iter",
            "extra": "iterations: 168\ncpu: 4135.981000000164 us\nthreads: 1"
          },
          {
            "name": "ZK/UCElGamalComMultPrivScalar/Verify/3",
            "value": 1025.0129546786454,
            "unit": "us/iter",
            "extra": "iterations: 684\ncpu: 1024.8830321637936 us\nthreads: 1"
          },
          {
            "name": "ZK/ValidPaillier/Prover",
            "value": 21198.024787874157,
            "unit": "us/iter",
            "extra": "iterations: 33\ncpu: 21195.95275757485 us\nthreads: 1"
          },
          {
            "name": "ZK/ValidPaillier/Verify",
            "value": 20417.17747058957,
            "unit": "us/iter",
            "extra": "iterations: 34\ncpu: 20416.07347058683 us\nthreads: 1"
          },
          {
            "name": "ZK/PaillierZero/Prover",
            "value": 85759.08237497743,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 85749.03575000548 us\nthreads: 1"
          },
          {
            "name": "ZK/PaillierZero/Verify",
            "value": 81371.55362499016,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 81362.10224999729 us\nthreads: 1"
          },
          {
            "name": "ZK/TwoPaillierEqual/Prover",
            "value": 82086.59087500791,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 82081.12637500165 us\nthreads: 1"
          },
          {
            "name": "ZK/TwoPaillierEqual/Verify",
            "value": 167575.54175001133,
            "unit": "us/iter",
            "extra": "iterations: 4\ncpu: 167561.83050000572 us\nthreads: 1"
          },
          {
            "name": "ZK/RangePedersen/Prover/3",
            "value": 299912.4294999547,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 299893.61099998746 us\nthreads: 1"
          },
          {
            "name": "ZK/RangePedersen/Verify/3",
            "value": 16117.167953494614,
            "unit": "us/iter",
            "extra": "iterations: 43\ncpu: 16115.63413953501 us\nthreads: 1"
          },
          {
            "name": "ZK/PaillierPedersenEq/Prover/3",
            "value": 43577.77875000579,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 43572.014437497586 us\nthreads: 1"
          },
          {
            "name": "ZK/PaillierPedersenEq/Verify/3",
            "value": 90018.51312501685,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 90007.30700000048 us\nthreads: 1"
          },
          {
            "name": "ZK/PaillierRangeExpSlack/Prover/3",
            "value": 345760.4150000861,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 345702.6929999927 us\nthreads: 1"
          },
          {
            "name": "ZK/PaillierRangeExpSlack/Verify/3",
            "value": 107988.3007142988,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 107972.64185714538 us\nthreads: 1"
          },
          {
            "name": "ZK/PDL/Prover",
            "value": 350056.4004998523,
            "unit": "us/iter",
            "extra": "iterations: 2\ncpu: 350027.61100000155 us\nthreads: 1"
          },
          {
            "name": "ZK/PDL/Verify",
            "value": 116944.92500002222,
            "unit": "us/iter",
            "extra": "iterations: 6\ncpu: 116935.05150000761 us\nthreads: 1"
          },
          {
            "name": "ZK/UnknownOrderDL/Prove",
            "value": 267724.5299999716,
            "unit": "us/iter",
            "extra": "iterations: 3\ncpu: 267715.05766667286 us\nthreads: 1"
          },
          {
            "name": "ZK/UnknownOrderDL/Verify",
            "value": 269253.37633338134,
            "unit": "us/iter",
            "extra": "iterations: 3\ncpu: 269237.84533333144 us\nthreads: 1"
          }
        ]
      }
    ]
  }
}