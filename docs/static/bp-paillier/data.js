window.BENCHMARK_DATA = {
  "lastUpdate": 1752241165297,
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
        "date": 1742577982896,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 50677.50133332538,
            "unit": "us/iter",
            "extra": "iterations: 12\ncpu: 50674.83366666667 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4510.268438711899,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4510.072058064515 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8584.010170735137,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8582.526256097559 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4524.982877419168,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4524.702199999997 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.027765286831732,
            "unit": "us/iter",
            "extra": "iterations: 63486\ncpu: 11.026958211259174 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 13.040747751883424,
            "unit": "us/iter",
            "extra": "iterations: 54490\ncpu: 13.040027069187001 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8316.324238096839,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8315.720809523813 us\nthreads: 1"
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
        "date": 1742998245618,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 58909.9236470461,
            "unit": "us/iter",
            "extra": "iterations: 17\ncpu: 58906.11223529412 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4533.6724155861575,
            "unit": "us/iter",
            "extra": "iterations: 154\ncpu: 4533.391642857143 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8571.411950616033,
            "unit": "us/iter",
            "extra": "iterations: 81\ncpu: 8571.125172839505 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4527.029290323798,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4526.520645161295 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.128012522614132,
            "unit": "us/iter",
            "extra": "iterations: 63006\ncpu: 11.127469320382183 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.850512530064387,
            "unit": "us/iter",
            "extra": "iterations: 54469\ncpu: 12.848788044575821 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8323.063285713162,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8320.815785714292 us\nthreads: 1"
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
        "date": 1746512430114,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 60140.76599999498,
            "unit": "us/iter",
            "extra": "iterations: 10\ncpu: 60136.13710000001 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4513.253161290308,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4512.994432258063 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8557.377414634662,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8556.884792682928 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4500.025677419128,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4499.731709677418 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.046545496240894,
            "unit": "us/iter",
            "extra": "iterations: 63225\ncpu: 11.045676615262959 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.78564396641748,
            "unit": "us/iter",
            "extra": "iterations: 54669\ncpu: 12.78454707421024 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8318.898964283844,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8318.350392857137 us\nthreads: 1"
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
        "date": 1746541579765,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 44971.63306250229,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 44965.390250000004 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4514.073954838281,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4513.4220774193545 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8574.59604878009,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8573.51118292683 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4507.924025806314,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4507.347587096777 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.122463114558974,
            "unit": "us/iter",
            "extra": "iterations: 63399\ncpu: 11.12076326125018 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.81618958413544,
            "unit": "us/iter",
            "extra": "iterations: 54609\ncpu: 12.814603087403174 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8374.6170119038,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8373.667238095237 us\nthreads: 1"
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
        "date": 1746544410014,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 49553.290352939846,
            "unit": "us/iter",
            "extra": "iterations: 17\ncpu: 49547.30300000001 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4514.022535947327,
            "unit": "us/iter",
            "extra": "iterations: 153\ncpu: 4513.76677124183 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8566.131036587183,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8564.867756097563 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4501.10781290248,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4501.053948387097 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.042495684962136,
            "unit": "us/iter",
            "extra": "iterations: 63383\ncpu: 11.041912089992591 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.795051484204667,
            "unit": "us/iter",
            "extra": "iterations: 54541\ncpu: 12.793535285381639 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8323.17454761605,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8322.434726190468 us\nthreads: 1"
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
        "date": 1746625696908,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 39363.290199995994,
            "unit": "us/iter",
            "extra": "iterations: 15\ncpu: 39360.849733333336 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4511.582109677375,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4510.954129032257 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8564.847804877456,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8564.55119512195 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4500.486897434724,
            "unit": "us/iter",
            "extra": "iterations: 156\ncpu: 4499.959621794873 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.01972305793887,
            "unit": "us/iter",
            "extra": "iterations: 63219\ncpu: 11.018196665559413 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.825125633770496,
            "unit": "us/iter",
            "extra": "iterations: 54436\ncpu: 12.824073774707912 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8319.172440478507,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8318.212035714283 us\nthreads: 1"
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
        "date": 1746628454668,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 45570.44354545393,
            "unit": "us/iter",
            "extra": "iterations: 11\ncpu: 45567.12290909091 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4512.240451614132,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4511.707032258064 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8574.397109752194,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8573.558560975609 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4510.661380643269,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4510.360432258065 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.023678769942276,
            "unit": "us/iter",
            "extra": "iterations: 63509\ncpu: 11.022226456092834 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.831933150956843,
            "unit": "us/iter",
            "extra": "iterations: 54466\ncpu: 12.831045386112434 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8329.465916663788,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8328.269607142862 us\nthreads: 1"
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
        "date": 1746632564861,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 45065.71627777905,
            "unit": "us/iter",
            "extra": "iterations: 18\ncpu: 45063.23383333333 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4520.631425805644,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4520.337838709676 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8567.43902439054,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8567.00007317073 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4507.8451290318035,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4507.722877419352 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.083187838566044,
            "unit": "us/iter",
            "extra": "iterations: 63134\ncpu: 11.080738112585921 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.879516654410516,
            "unit": "us/iter",
            "extra": "iterations: 54400\ncpu: 12.87914516544119 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8326.565607143537,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8326.29253571429 us\nthreads: 1"
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
        "date": 1746638652969,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 55144.30350001476,
            "unit": "us/iter",
            "extra": "iterations: 12\ncpu: 55138.673 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4517.251470968169,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4516.967690322582 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8574.949625000272,
            "unit": "us/iter",
            "extra": "iterations: 80\ncpu: 8574.485275 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4528.255651612492,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4527.751606451611 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.586435686982167,
            "unit": "us/iter",
            "extra": "iterations: 60431\ncpu: 11.585846469527224 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 13.416013134866756,
            "unit": "us/iter",
            "extra": "iterations: 51999\ncpu: 13.4147881497721 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8317.634654761583,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8317.308440476188 us\nthreads: 1"
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
        "date": 1749827760604,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 57691.18068752732,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 57688.532 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4519.8814387086095,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4519.590941935484 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8568.689060977216,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8568.522707317074 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4618.674664514754,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4618.280922580648 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.579555172817198,
            "unit": "us/iter",
            "extra": "iterations: 60383\ncpu: 11.579317556265837 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 13.398695787118603,
            "unit": "us/iter",
            "extra": "iterations: 52197\ncpu: 13.397940667088161 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8346.721719512638,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8346.399231707323 us\nthreads: 1"
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
        "date": 1749921459593,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 58599.31831578012,
            "unit": "us/iter",
            "extra": "iterations: 19\ncpu: 58591.42536842105 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4515.479529031689,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4514.95895483871 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8558.220646341952,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8557.943134146344 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4516.255116129375,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4515.823574193546 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.012066133941389,
            "unit": "us/iter",
            "extra": "iterations: 63447\ncpu: 11.011125348716257 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.813641250428283,
            "unit": "us/iter",
            "extra": "iterations: 54637\ncpu: 12.81299888354045 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8329.374535714167,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8328.026738095237 us\nthreads: 1"
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
        "date": 1750176292125,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 44687.29235713909,
            "unit": "us/iter",
            "extra": "iterations: 14\ncpu: 44682.55471428572 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4511.142574196011,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4510.774161290324 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8577.591926828838,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8576.86074390244 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4505.112735484046,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4504.98069032258 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.041194142667242,
            "unit": "us/iter",
            "extra": "iterations: 63237\ncpu: 11.040466467416236 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.89370993450973,
            "unit": "us/iter",
            "extra": "iterations: 54205\ncpu: 12.892964523567928 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8322.761084340285,
            "unit": "us/iter",
            "extra": "iterations: 83\ncpu: 8322.320903614447 us\nthreads: 1"
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
        "date": 1750863418711,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 60341.758374988785,
            "unit": "us/iter",
            "extra": "iterations: 16\ncpu: 60336.51775 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4511.030219354354,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4510.535535483871 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8564.077170731474,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8563.731743902443 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4507.022993549336,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4506.793587096775 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.572580948434783,
            "unit": "us/iter",
            "extra": "iterations: 60310\ncpu: 11.572097347040286 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 13.425945784746624,
            "unit": "us/iter",
            "extra": "iterations: 52144\ncpu: 13.425365986498923 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8324.878156627274,
            "unit": "us/iter",
            "extra": "iterations: 83\ncpu: 8324.734662650604 us\nthreads: 1"
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
        "date": 1752071928183,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 72881.78312498417,
            "unit": "us/iter",
            "extra": "iterations: 8\ncpu: 72880.14600000001 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4515.407954838133,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4515.091516129031 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8565.272768292722,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8564.981414634145 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4504.869870967717,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4504.730238709674 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 10.998039081545143,
            "unit": "us/iter",
            "extra": "iterations: 63585\ncpu: 10.996983124950841 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.818252424504424,
            "unit": "us/iter",
            "extra": "iterations: 54341\ncpu: 12.817521190261488 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8320.895285713787,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8320.114023809516 us\nthreads: 1"
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
        "date": 1752241164448,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "BP/Paillier/Gen",
            "value": 62199.445600003855,
            "unit": "us/iter",
            "extra": "iterations: 10\ncpu: 62197.76250000001 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Enc",
            "value": 4510.910638710782,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4510.764703225806 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Pub-Enc",
            "value": 8575.980914634429,
            "unit": "us/iter",
            "extra": "iterations: 82\ncpu: 8575.016109756098 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Dec",
            "value": 4513.576200000193,
            "unit": "us/iter",
            "extra": "iterations: 155\ncpu: 4513.554290322583 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add",
            "value": 11.125948061621774,
            "unit": "us/iter",
            "extra": "iterations: 63094\ncpu: 11.12464784290106 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Add-Scalar",
            "value": 12.874430180180338,
            "unit": "us/iter",
            "extra": "iterations: 54168\ncpu: 12.87335893885689 us\nthreads: 1"
          },
          {
            "name": "BP/Paillier/Mul-Scalar",
            "value": 8318.606166667343,
            "unit": "us/iter",
            "extra": "iterations: 84\ncpu: 8317.422797619045 us\nthreads: 1"
          }
        ]
      }
    ]
  }
}