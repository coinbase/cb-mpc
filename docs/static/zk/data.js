window.BENCHMARK_DATA = {
  "lastUpdate": 1746632567035,
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
      }
    ]
  }
}