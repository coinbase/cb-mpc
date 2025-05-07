window.BENCHMARK_DATA = {
  "lastUpdate": 1746625701154,
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
        "date": 1742577986140,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24440.720551724397,
            "unit": "us/iter",
            "extra": "iterations: 29\ncpu: 24439.121482758626 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 99063.5975713953,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 99062.52885714288 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13775.621254897524,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13774.46854901961 us\nthreads: 1"
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
        "date": 1742998249764,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24493.624071429364,
            "unit": "us/iter",
            "extra": "iterations: 28\ncpu: 24491.654785714287 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 99203.63271430686,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 99173.97014285716 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13820.407823530724,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13819.068882352942 us\nthreads: 1"
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
        "date": 1746512433952,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24442.108896562724,
            "unit": "us/iter",
            "extra": "iterations: 29\ncpu: 24438.857103448274 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 98865.46857140794,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 98855.76185714288 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13782.67821569263,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13780.601333333329 us\nthreads: 1"
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
        "date": 1746541582532,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24376.948379320875,
            "unit": "us/iter",
            "extra": "iterations: 29\ncpu: 24375.263206896558 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 98937.16042857444,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 98928.55142857145 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13817.080137262867,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13815.211078431374 us\nthreads: 1"
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
        "date": 1746544412902,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24375.38682758306,
            "unit": "us/iter",
            "extra": "iterations: 29\ncpu: 24374.066689655174 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 98951.68285714005,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 98940.83085714285 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13791.27956862944,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13790.933686274508 us\nthreads: 1"
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
        "date": 1746625700318,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "MPC/OT/BaseOT/Step1_R2S/256",
            "value": 24412.70886207537,
            "unit": "us/iter",
            "extra": "iterations: 29\ncpu: 24409.450655172415 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/Step2_S2R/256",
            "value": 98757.04599997854,
            "unit": "us/iter",
            "extra": "iterations: 7\ncpu: 98748.3255714286 us\nthreads: 1"
          },
          {
            "name": "MPC/OT/BaseOT/OutputR/256",
            "value": 13770.792921569213,
            "unit": "us/iter",
            "extra": "iterations: 51\ncpu: 13768.312823529415 us\nthreads: 1"
          }
        ]
      }
    ]
  }
}