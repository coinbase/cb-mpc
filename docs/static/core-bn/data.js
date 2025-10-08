window.BENCHMARK_DATA = {
  "lastUpdate": 1759938066760,
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
        "date": 1742577976780,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/BN/ModAdd/256",
            "value": 0.0689495747207143,
            "unit": "us/iter",
            "extra": "iterations: 10785265\ncpu: 0.06889960024162597 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModAdd/4096",
            "value": 0.27184722132440914,
            "unit": "us/iter",
            "extra": "iterations: 2614213\ncpu: 0.2718297154822526 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/256",
            "value": 0.05956496115758171,
            "unit": "us/iter",
            "extra": "iterations: 11736906\ncpu: 0.05955836947147712 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/4096",
            "value": 0.3219511270269253,
            "unit": "us/iter",
            "extra": "iterations: 2162770\ncpu: 0.3219346023849032 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/256",
            "value": 0.15217459046637014,
            "unit": "us/iter",
            "extra": "iterations: 4542728\ncpu: 0.1521660658529419 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/4096",
            "value": 10.944231997354326,
            "unit": "us/iter",
            "extra": "iterations: 63449\ncpu: 10.943258286182495 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/256",
            "value": 16.30634100135384,
            "unit": "us/iter",
            "extra": "iterations: 42862\ncpu: 16.30560013531778 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/4096",
            "value": 16514.209857145295,
            "unit": "us/iter",
            "extra": "iterations: 42\ncpu: 16513.879595237988 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/256",
            "value": 17.345175812381814,
            "unit": "us/iter",
            "extra": "iterations: 40037\ncpu: 17.340283287958737 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/4096",
            "value": 724.7319522821084,
            "unit": "us/iter",
            "extra": "iterations: 964\ncpu: 724.7079273858892 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/256",
            "value": 23.262983009062314,
            "unit": "us/iter",
            "extra": "iterations: 30428\ncpu: 23.261372288681088 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/4096",
            "value": 1132.6749067794187,
            "unit": "us/iter",
            "extra": "iterations: 590\ncpu: 1132.6313644067766 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/256",
            "value": 22.914513218118508,
            "unit": "us/iter",
            "extra": "iterations: 30375\ncpu: 22.911947456790166 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/4096",
            "value": 1138.4899515344434,
            "unit": "us/iter",
            "extra": "iterations: 619\ncpu: 1138.389878836821 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/256",
            "value": 25.857362074060717,
            "unit": "us/iter",
            "extra": "iterations: 27058\ncpu: 25.8562280286794 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/4096",
            "value": 1306.3884283053815,
            "unit": "us/iter",
            "extra": "iterations: 537\ncpu: 1306.3007094972006 us\nthreads: 1"
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
        "date": 1742998238325,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/BN/ModAdd/256",
            "value": 0.07772082540773526,
            "unit": "us/iter",
            "extra": "iterations: 9798406\ncpu: 0.07771418238844155 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModAdd/4096",
            "value": 0.2792133440718488,
            "unit": "us/iter",
            "extra": "iterations: 2540394\ncpu: 0.279199963076594 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/256",
            "value": 0.06476722215326824,
            "unit": "us/iter",
            "extra": "iterations: 10666204\ncpu: 0.06476310944362258 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/4096",
            "value": 0.32143477552106087,
            "unit": "us/iter",
            "extra": "iterations: 2128641\ncpu: 0.32140257939220573 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/256",
            "value": 0.16341552131948753,
            "unit": "us/iter",
            "extra": "iterations: 4284945\ncpu: 0.1634097000078184 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/4096",
            "value": 11.002197134590894,
            "unit": "us/iter",
            "extra": "iterations: 63586\ncpu: 11.001557544113654 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/256",
            "value": 16.171087651147737,
            "unit": "us/iter",
            "extra": "iterations: 43251\ncpu: 16.170508658759328 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/4096",
            "value": 16539.487428569068,
            "unit": "us/iter",
            "extra": "iterations: 42\ncpu: 16538.927476190678 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/256",
            "value": 17.02766625247389,
            "unit": "us/iter",
            "extra": "iterations: 41043\ncpu: 17.027287576443914 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/4096",
            "value": 682.1525708739536,
            "unit": "us/iter",
            "extra": "iterations: 1030\ncpu: 682.0820300970851 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/256",
            "value": 23.28301472202193,
            "unit": "us/iter",
            "extra": "iterations: 30974\ncpu: 23.280165041647404 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/4096",
            "value": 1148.2240146343554,
            "unit": "us/iter",
            "extra": "iterations: 615\ncpu: 1148.0999804877972 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/256",
            "value": 23.611830797593267,
            "unit": "us/iter",
            "extra": "iterations: 30366\ncpu: 23.609616841203238 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/4096",
            "value": 1218.0488529412921,
            "unit": "us/iter",
            "extra": "iterations: 612\ncpu: 1217.9957859477042 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/256",
            "value": 25.875947820296926,
            "unit": "us/iter",
            "extra": "iterations: 27022\ncpu: 25.874558544889165 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/4096",
            "value": 1311.0446841127557,
            "unit": "us/iter",
            "extra": "iterations: 535\ncpu: 1311.0016579439 us\nthreads: 1"
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
        "date": 1746512422672,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/BN/ModAdd/256",
            "value": 0.06671779556904975,
            "unit": "us/iter",
            "extra": "iterations: 10852016\ncpu: 0.06670421486662018 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModAdd/4096",
            "value": 0.2753565177477668,
            "unit": "us/iter",
            "extra": "iterations: 2601259\ncpu: 0.2753409329866787 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/256",
            "value": 0.06348063044549646,
            "unit": "us/iter",
            "extra": "iterations: 11002886\ncpu: 0.06347722815632206 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/4096",
            "value": 0.3276833366481484,
            "unit": "us/iter",
            "extra": "iterations: 2151992\ncpu: 0.3276601325655493 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/256",
            "value": 0.15182206903568132,
            "unit": "us/iter",
            "extra": "iterations: 4621090\ncpu: 0.1518154861731746 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/4096",
            "value": 10.979652852739179,
            "unit": "us/iter",
            "extra": "iterations: 63921\ncpu: 10.979204001814752 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/256",
            "value": 16.306039292228625,
            "unit": "us/iter",
            "extra": "iterations: 42782\ncpu: 16.305259431536808 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/4096",
            "value": 16527.26726190611,
            "unit": "us/iter",
            "extra": "iterations: 42\ncpu: 16525.71623809522 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/256",
            "value": 16.6727226834761,
            "unit": "us/iter",
            "extra": "iterations: 42035\ncpu: 16.67145195670291 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/4096",
            "value": 660.4152768362591,
            "unit": "us/iter",
            "extra": "iterations: 1062\ncpu: 660.3645160075419 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/256",
            "value": 23.73499427029182,
            "unit": "us/iter",
            "extra": "iterations: 30019\ncpu: 23.73385665745025 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/4096",
            "value": 1145.7991964287478,
            "unit": "us/iter",
            "extra": "iterations: 616\ncpu: 1145.7000649350384 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/256",
            "value": 23.760400155902072,
            "unit": "us/iter",
            "extra": "iterations: 29506\ncpu: 23.75939083576204 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/4096",
            "value": 1142.280784440915,
            "unit": "us/iter",
            "extra": "iterations: 617\ncpu: 1142.2098800648166 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/256",
            "value": 26.079664015245644,
            "unit": "us/iter",
            "extra": "iterations: 27281\ncpu: 26.07776954657059 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/4096",
            "value": 1302.2960655432353,
            "unit": "us/iter",
            "extra": "iterations: 534\ncpu: 1302.2178801498212 us\nthreads: 1"
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
        "date": 1746541574120,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/BN/ModAdd/256",
            "value": 0.06478839717857172,
            "unit": "us/iter",
            "extra": "iterations: 10007428\ncpu: 0.06478630872987544 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModAdd/4096",
            "value": 0.26893876092237223,
            "unit": "us/iter",
            "extra": "iterations: 2603501\ncpu: 0.26891235609281255 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/256",
            "value": 0.06395840965661657,
            "unit": "us/iter",
            "extra": "iterations: 10945762\ncpu: 0.06395495462079312 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/4096",
            "value": 0.3207278645833543,
            "unit": "us/iter",
            "extra": "iterations: 2168832\ncpu: 0.32070061074347717 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/256",
            "value": 0.15225004764582956,
            "unit": "us/iter",
            "extra": "iterations: 4606910\ncpu: 0.15224424375557663 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/4096",
            "value": 10.955350831064875,
            "unit": "us/iter",
            "extra": "iterations: 63894\ncpu: 10.954635881303618 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/256",
            "value": 16.340201231887853,
            "unit": "us/iter",
            "extra": "iterations: 42861\ncpu: 16.338200508620762 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/4096",
            "value": 16522.19854762344,
            "unit": "us/iter",
            "extra": "iterations: 42\ncpu: 16520.95585714281 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/256",
            "value": 16.71655408141068,
            "unit": "us/iter",
            "extra": "iterations: 41983\ncpu: 16.715700974203823 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/4096",
            "value": 659.7532422243993,
            "unit": "us/iter",
            "extra": "iterations: 1061\ncpu: 659.7069905749186 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/256",
            "value": 23.26147108754508,
            "unit": "us/iter",
            "extra": "iterations: 30748\ncpu: 23.258978177442053 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/4096",
            "value": 1136.0106094005496,
            "unit": "us/iter",
            "extra": "iterations: 617\ncpu: 1135.820097244707 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/256",
            "value": 22.86170892111567,
            "unit": "us/iter",
            "extra": "iterations: 30198\ncpu: 22.85988068746219 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/4096",
            "value": 1129.507996705111,
            "unit": "us/iter",
            "extra": "iterations: 607\ncpu: 1129.4336392092403 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/256",
            "value": 25.242962667854663,
            "unit": "us/iter",
            "extra": "iterations: 27108\ncpu: 25.241724583149125 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/4096",
            "value": 1306.1488632161577,
            "unit": "us/iter",
            "extra": "iterations: 541\ncpu: 1306.0216894639564 us\nthreads: 1"
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
        "date": 1746544403979,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/BN/ModAdd/256",
            "value": 0.06574656968886887,
            "unit": "us/iter",
            "extra": "iterations: 10907101\ncpu: 0.0657388420626159 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModAdd/4096",
            "value": 0.27056017161013474,
            "unit": "us/iter",
            "extra": "iterations: 2600080\ncpu: 0.2705305786745027 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/256",
            "value": 0.06386917670018778,
            "unit": "us/iter",
            "extra": "iterations: 10928619\ncpu: 0.06386332271259537 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/4096",
            "value": 0.3222509764063329,
            "unit": "us/iter",
            "extra": "iterations: 2127188\ncpu: 0.32223048691512207 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/256",
            "value": 0.1528671321363223,
            "unit": "us/iter",
            "extra": "iterations: 4583328\ncpu: 0.15285484324927373 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/4096",
            "value": 10.961626780713964,
            "unit": "us/iter",
            "extra": "iterations: 63949\ncpu: 10.960762107304184 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/256",
            "value": 16.369351820804816,
            "unit": "us/iter",
            "extra": "iterations: 42948\ncpu: 16.36700230511332 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/4096",
            "value": 16526.15204761787,
            "unit": "us/iter",
            "extra": "iterations: 42\ncpu: 16525.833595238062 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/256",
            "value": 16.683994684005487,
            "unit": "us/iter",
            "extra": "iterations: 42137\ncpu: 16.682227116311104 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/4096",
            "value": 654.2242497652352,
            "unit": "us/iter",
            "extra": "iterations: 1065\ncpu: 654.1424422535312 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/256",
            "value": 22.971383518687738,
            "unit": "us/iter",
            "extra": "iterations: 30932\ncpu: 22.971467380059053 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/4096",
            "value": 1129.183293354758,
            "unit": "us/iter",
            "extra": "iterations: 617\ncpu: 1129.1449967585122 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/256",
            "value": 23.274729145415147,
            "unit": "us/iter",
            "extra": "iterations: 30389\ncpu: 23.274168120043882 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/4096",
            "value": 1129.3996584159422,
            "unit": "us/iter",
            "extra": "iterations: 606\ncpu: 1129.364222772295 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/256",
            "value": 25.726572095040172,
            "unit": "us/iter",
            "extra": "iterations: 27651\ncpu: 25.724467867346636 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/4096",
            "value": 1303.2130111523184,
            "unit": "us/iter",
            "extra": "iterations: 538\ncpu: 1303.15332156134 us\nthreads: 1"
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
        "date": 1746625690484,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/BN/ModAdd/256",
            "value": 0.06752395535258171,
            "unit": "us/iter",
            "extra": "iterations: 10319343\ncpu: 0.06752084740278523 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModAdd/4096",
            "value": 0.2677960511720729,
            "unit": "us/iter",
            "extra": "iterations: 2569218\ncpu: 0.26777337578982974 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/256",
            "value": 0.06414505907973111,
            "unit": "us/iter",
            "extra": "iterations: 10930312\ncpu: 0.06414215559446096 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/4096",
            "value": 0.3212565809656053,
            "unit": "us/iter",
            "extra": "iterations: 2161271\ncpu: 0.32122184353558425 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/256",
            "value": 0.15830090987196807,
            "unit": "us/iter",
            "extra": "iterations: 4439306\ncpu: 0.1582941403453608 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/4096",
            "value": 10.916991429330091,
            "unit": "us/iter",
            "extra": "iterations: 63939\ncpu: 10.916419868937696 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/256",
            "value": 16.34663879567188,
            "unit": "us/iter",
            "extra": "iterations: 42613\ncpu: 16.34646347358799 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/4096",
            "value": 16561.03723809381,
            "unit": "us/iter",
            "extra": "iterations: 42\ncpu: 16560.242071428598 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/256",
            "value": 17.009368972386167,
            "unit": "us/iter",
            "extra": "iterations: 41144\ncpu: 17.007884260159603 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/4096",
            "value": 666.7312288698915,
            "unit": "us/iter",
            "extra": "iterations: 1053\ncpu: 666.6569449192785 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/256",
            "value": 23.3051178273652,
            "unit": "us/iter",
            "extra": "iterations: 30341\ncpu: 23.30451863814608 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/4096",
            "value": 1238.7688294699958,
            "unit": "us/iter",
            "extra": "iterations: 604\ncpu: 1238.7197003311283 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/256",
            "value": 22.788043881965823,
            "unit": "us/iter",
            "extra": "iterations: 30263\ncpu: 22.787545682847586 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/4096",
            "value": 1163.6139041321476,
            "unit": "us/iter",
            "extra": "iterations: 605\ncpu: 1163.5147487603049 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/256",
            "value": 25.12491651099649,
            "unit": "us/iter",
            "extra": "iterations: 27836\ncpu: 25.123745329788687 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/4096",
            "value": 1324.2251730038397,
            "unit": "us/iter",
            "extra": "iterations: 526\ncpu: 1324.1454657794616 us\nthreads: 1"
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
        "date": 1746628449370,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/BN/ModAdd/256",
            "value": 0.06809098601608299,
            "unit": "us/iter",
            "extra": "iterations: 10411534\ncpu: 0.06808597023262854 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModAdd/4096",
            "value": 0.2680365103797892,
            "unit": "us/iter",
            "extra": "iterations: 2539497\ncpu: 0.26802913962883224 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/256",
            "value": 0.06403904035363095,
            "unit": "us/iter",
            "extra": "iterations: 11003845\ncpu: 0.06403619343965665 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/4096",
            "value": 0.32542037859195033,
            "unit": "us/iter",
            "extra": "iterations: 2172682\ncpu: 0.32539967100569633 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/256",
            "value": 0.1592505905942341,
            "unit": "us/iter",
            "extra": "iterations: 4411235\ncpu: 0.1592380596363598 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/4096",
            "value": 10.9252629626716,
            "unit": "us/iter",
            "extra": "iterations: 64165\ncpu: 10.92455787423042 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/256",
            "value": 16.357744682334637,
            "unit": "us/iter",
            "extra": "iterations: 42970\ncpu: 16.356927065394633 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/4096",
            "value": 16541.441738094625,
            "unit": "us/iter",
            "extra": "iterations: 42\ncpu: 16540.634238095226 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/256",
            "value": 16.97885254938414,
            "unit": "us/iter",
            "extra": "iterations: 41363\ncpu: 16.977488963566305 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/4096",
            "value": 670.7864482756311,
            "unit": "us/iter",
            "extra": "iterations: 1044\ncpu: 670.711344827593 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/256",
            "value": 22.791102239604427,
            "unit": "us/iter",
            "extra": "iterations: 30184\ncpu: 22.788687185263743 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/4096",
            "value": 1253.3051766664964,
            "unit": "us/iter",
            "extra": "iterations: 600\ncpu: 1253.1842583333248 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/256",
            "value": 22.954974689824596,
            "unit": "us/iter",
            "extra": "iterations: 30225\ncpu: 22.952437253929023 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/4096",
            "value": 1168.246745424304,
            "unit": "us/iter",
            "extra": "iterations: 601\ncpu: 1168.2170083194303 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/256",
            "value": 25.77239212378883,
            "unit": "us/iter",
            "extra": "iterations: 27272\ncpu: 25.771325755353473 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/4096",
            "value": 1332.314742857783,
            "unit": "us/iter",
            "extra": "iterations: 525\ncpu: 1332.2346609524109 us\nthreads: 1"
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
        "date": 1746632558897,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/BN/ModAdd/256",
            "value": 0.06475528470009345,
            "unit": "us/iter",
            "extra": "iterations: 10882311\ncpu: 0.06474868509087822 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModAdd/4096",
            "value": 0.27381743044226675,
            "unit": "us/iter",
            "extra": "iterations: 2544181\ncpu: 0.27380134039205395 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/256",
            "value": 0.06087970177080102,
            "unit": "us/iter",
            "extra": "iterations: 11412296\ncpu: 0.06087721603084985 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/4096",
            "value": 0.32330247664744516,
            "unit": "us/iter",
            "extra": "iterations: 2165387\ncpu: 0.32329555409725824 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/256",
            "value": 0.14879768043831215,
            "unit": "us/iter",
            "extra": "iterations: 4620873\ncpu: 0.14878277135077964 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/4096",
            "value": 10.99735839587261,
            "unit": "us/iter",
            "extra": "iterations: 63561\ncpu: 10.996334151444978 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/256",
            "value": 16.28519077931041,
            "unit": "us/iter",
            "extra": "iterations: 42730\ncpu: 16.28375431780949 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/4096",
            "value": 16528.612023811158,
            "unit": "us/iter",
            "extra": "iterations: 42\ncpu: 16526.395809523685 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/256",
            "value": 16.978611256385115,
            "unit": "us/iter",
            "extra": "iterations: 40919\ncpu: 16.977506366235943 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/4096",
            "value": 671.5647643679868,
            "unit": "us/iter",
            "extra": "iterations: 1044\ncpu: 671.532265325667 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/256",
            "value": 22.550582046149028,
            "unit": "us/iter",
            "extra": "iterations: 30946\ncpu: 22.548413203645044 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/4096",
            "value": 1205.911627986057,
            "unit": "us/iter",
            "extra": "iterations: 586\ncpu: 1205.8000529010374 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/256",
            "value": 22.861824474091666,
            "unit": "us/iter",
            "extra": "iterations: 30138\ncpu: 22.86125880947595 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/4096",
            "value": 1213.0897482757848,
            "unit": "us/iter",
            "extra": "iterations: 580\ncpu: 1213.0732758620866 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/256",
            "value": 25.825116242867786,
            "unit": "us/iter",
            "extra": "iterations: 26978\ncpu: 25.821381607235665 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/4096",
            "value": 1363.3338070173982,
            "unit": "us/iter",
            "extra": "iterations: 513\ncpu: 1363.2869688109117 us\nthreads: 1"
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
        "date": 1746638647543,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/BN/ModAdd/256",
            "value": 0.06587689825765052,
            "unit": "us/iter",
            "extra": "iterations: 10859432\ncpu: 0.06587006889494773 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModAdd/4096",
            "value": 0.2735669216814623,
            "unit": "us/iter",
            "extra": "iterations: 2562204\ncpu: 0.27353761878445215 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/256",
            "value": 0.06538548826954513,
            "unit": "us/iter",
            "extra": "iterations: 10704292\ncpu: 0.06538156872028546 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/4096",
            "value": 0.33790045398245544,
            "unit": "us/iter",
            "extra": "iterations: 2144576\ncpu: 0.33788704667029607 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/256",
            "value": 0.15348537353773756,
            "unit": "us/iter",
            "extra": "iterations: 4558040\ncpu: 0.153473769865995 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/4096",
            "value": 11.494585220266405,
            "unit": "us/iter",
            "extra": "iterations: 60813\ncpu: 11.493724746353486 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/256",
            "value": 16.28734419253482,
            "unit": "us/iter",
            "extra": "iterations: 43005\ncpu: 16.286271480060325 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/4096",
            "value": 16534.85023809069,
            "unit": "us/iter",
            "extra": "iterations: 42\ncpu: 16533.818357142343 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/256",
            "value": 19.137854097012042,
            "unit": "us/iter",
            "extra": "iterations: 36490\ncpu: 19.137527295148644 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/4096",
            "value": 699.2491377245361,
            "unit": "us/iter",
            "extra": "iterations: 1002\ncpu: 699.2208532934088 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/256",
            "value": 22.48592913841338,
            "unit": "us/iter",
            "extra": "iterations: 31117\ncpu: 22.4853091236302 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/4096",
            "value": 1210.767740103338,
            "unit": "us/iter",
            "extra": "iterations: 581\ncpu: 1210.6833666093214 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/256",
            "value": 23.268269523967536,
            "unit": "us/iter",
            "extra": "iterations: 30565\ncpu: 23.267409716996447 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/4096",
            "value": 1205.9196883561146,
            "unit": "us/iter",
            "extra": "iterations: 584\ncpu: 1205.8647842466062 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/256",
            "value": 25.474187644758462,
            "unit": "us/iter",
            "extra": "iterations: 27632\ncpu: 25.472863021134906 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/4096",
            "value": 1369.8752834644854,
            "unit": "us/iter",
            "extra": "iterations: 508\ncpu: 1369.7870472440973 us\nthreads: 1"
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
        "date": 1749827753880,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/BN/ModAdd/256",
            "value": 0.06853214104909915,
            "unit": "us/iter",
            "extra": "iterations: 10192480\ncpu: 0.06852663385162396 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModAdd/4096",
            "value": 0.2876522403629395,
            "unit": "us/iter",
            "extra": "iterations: 2514481\ncpu: 0.2876330419677066 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/256",
            "value": 0.06025025561493505,
            "unit": "us/iter",
            "extra": "iterations: 11545687\ncpu: 0.060246351646290525 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/4096",
            "value": 0.33755503967358713,
            "unit": "us/iter",
            "extra": "iterations: 2171470\ncpu: 0.33752291258916356 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/256",
            "value": 0.15391022500514534,
            "unit": "us/iter",
            "extra": "iterations: 4505230\ncpu: 0.15390022218621313 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/4096",
            "value": 11.485688538056092,
            "unit": "us/iter",
            "extra": "iterations: 60871\ncpu: 11.484626340950573 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/256",
            "value": 16.318341865780955,
            "unit": "us/iter",
            "extra": "iterations: 42856\ncpu: 16.316140937091866 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/4096",
            "value": 16530.66754761312,
            "unit": "us/iter",
            "extra": "iterations: 42\ncpu: 16529.377738095263 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/256",
            "value": 17.101034355682007,
            "unit": "us/iter",
            "extra": "iterations: 41245\ncpu: 17.098997405746385 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/4096",
            "value": 671.4214255318027,
            "unit": "us/iter",
            "extra": "iterations: 1034\ncpu: 671.3674090909144 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/256",
            "value": 22.791898435935764,
            "unit": "us/iter",
            "extra": "iterations: 30050\ncpu: 22.791161131447033 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/4096",
            "value": 1194.30669217447,
            "unit": "us/iter",
            "extra": "iterations: 575\ncpu: 1194.2504243478177 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/256",
            "value": 23.392681250207147,
            "unit": "us/iter",
            "extra": "iterations: 30651\ncpu: 23.392078888127973 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/4096",
            "value": 1203.5603367699798,
            "unit": "us/iter",
            "extra": "iterations: 582\ncpu: 1203.517646048082 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/256",
            "value": 25.688471360107116,
            "unit": "us/iter",
            "extra": "iterations: 27130\ncpu: 25.686843420567985 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/4096",
            "value": 1370.6892173913118,
            "unit": "us/iter",
            "extra": "iterations: 506\ncpu: 1370.5151363636126 us\nthreads: 1"
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
        "date": 1749921453245,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/BN/ModAdd/256",
            "value": 0.07057536791995829,
            "unit": "us/iter",
            "extra": "iterations: 10380179\ncpu: 0.07057363066667734 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModAdd/4096",
            "value": 0.27212334055306925,
            "unit": "us/iter",
            "extra": "iterations: 2570736\ncpu: 0.2720940687803017 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/256",
            "value": 0.06547744029440605,
            "unit": "us/iter",
            "extra": "iterations: 10693136\ncpu: 0.06547251479827815 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/4096",
            "value": 0.3227762435989526,
            "unit": "us/iter",
            "extra": "iterations: 2144180\ncpu: 0.32276808710089694 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/256",
            "value": 0.1494590604287496,
            "unit": "us/iter",
            "extra": "iterations: 4691842\ncpu: 0.1494418467203286 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/4096",
            "value": 11.012722793276742,
            "unit": "us/iter",
            "extra": "iterations: 64111\ncpu: 11.011261125235835 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/256",
            "value": 16.302070502863327,
            "unit": "us/iter",
            "extra": "iterations: 43133\ncpu: 16.300592168409587 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/4096",
            "value": 16537.359166672366,
            "unit": "us/iter",
            "extra": "iterations: 42\ncpu: 16535.21692857125 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/256",
            "value": 17.20927446602929,
            "unit": "us/iter",
            "extra": "iterations: 40311\ncpu: 17.207484036615597 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/4096",
            "value": 683.2468760974114,
            "unit": "us/iter",
            "extra": "iterations: 1025\ncpu: 683.2166360975574 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/256",
            "value": 22.825753844668583,
            "unit": "us/iter",
            "extra": "iterations: 30887\ncpu: 22.824437497976668 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/4096",
            "value": 1131.9640114752324,
            "unit": "us/iter",
            "extra": "iterations: 610\ncpu: 1131.9178442623397 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/256",
            "value": 22.884632464450394,
            "unit": "us/iter",
            "extra": "iterations: 30729\ncpu: 22.882383546487333 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/4096",
            "value": 1142.7565428111525,
            "unit": "us/iter",
            "extra": "iterations: 619\ncpu: 1142.6828675282597 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/256",
            "value": 24.859197982976045,
            "unit": "us/iter",
            "extra": "iterations: 27962\ncpu: 24.85774762177162 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/4096",
            "value": 1312.681917757613,
            "unit": "us/iter",
            "extra": "iterations: 535\ncpu: 1312.497586915869 us\nthreads: 1"
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
        "date": 1750176285347,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/BN/ModAdd/256",
            "value": 0.06845392755952082,
            "unit": "us/iter",
            "extra": "iterations: 10507963\ncpu: 0.06844896475177921 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModAdd/4096",
            "value": 0.2728705118594069,
            "unit": "us/iter",
            "extra": "iterations: 2573031\ncpu: 0.2728593639174968 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/256",
            "value": 0.07283155558996161,
            "unit": "us/iter",
            "extra": "iterations: 9714938\ncpu: 0.072826523854295 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/4096",
            "value": 0.32492819427674713,
            "unit": "us/iter",
            "extra": "iterations: 2156388\ncpu: 0.3249110136951271 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/256",
            "value": 0.15237856652813642,
            "unit": "us/iter",
            "extra": "iterations: 4585622\ncpu: 0.15237359293897143 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/4096",
            "value": 10.96361802413887,
            "unit": "us/iter",
            "extra": "iterations: 63881\ncpu: 10.963222476166612 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/256",
            "value": 16.386586728373622,
            "unit": "us/iter",
            "extra": "iterations: 42783\ncpu: 16.386169740317328 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/4096",
            "value": 16538.011333333696,
            "unit": "us/iter",
            "extra": "iterations: 42\ncpu: 16536.80661904761 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/256",
            "value": 17.86329209473052,
            "unit": "us/iter",
            "extra": "iterations: 39227\ncpu: 17.86244622326464 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/4096",
            "value": 664.2193444338004,
            "unit": "us/iter",
            "extra": "iterations: 1051\ncpu: 664.207460513789 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/256",
            "value": 22.580693744261822,
            "unit": "us/iter",
            "extra": "iterations: 30468\ncpu: 22.579655015097643 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/4096",
            "value": 1169.5776616666838,
            "unit": "us/iter",
            "extra": "iterations: 600\ncpu: 1169.5451600000222 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/256",
            "value": 23.333943581058104,
            "unit": "us/iter",
            "extra": "iterations: 30114\ncpu: 23.33300498107242 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/4096",
            "value": 1160.9880380796317,
            "unit": "us/iter",
            "extra": "iterations: 604\ncpu: 1160.9117930463515 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/256",
            "value": 25.599064574125652,
            "unit": "us/iter",
            "extra": "iterations: 27813\ncpu: 25.597566353863765 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/4096",
            "value": 1364.3056616258075,
            "unit": "us/iter",
            "extra": "iterations: 529\ncpu: 1364.1958752362752 us\nthreads: 1"
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
        "date": 1750863412380,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/BN/ModAdd/256",
            "value": 0.07573786000996457,
            "unit": "us/iter",
            "extra": "iterations: 9869345\ncpu: 0.07572805794102852 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModAdd/4096",
            "value": 0.2726749945790388,
            "unit": "us/iter",
            "extra": "iterations: 2587243\ncpu: 0.27267489795121597 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/256",
            "value": 0.06845309586773982,
            "unit": "us/iter",
            "extra": "iterations: 10271579\ncpu: 0.0684459933570099 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/4096",
            "value": 0.32336179226542267,
            "unit": "us/iter",
            "extra": "iterations: 2164300\ncpu: 0.3233539989835042 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/256",
            "value": 0.15100406768745261,
            "unit": "us/iter",
            "extra": "iterations: 4628920\ncpu: 0.15100126789834248 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/4096",
            "value": 11.504686855374294,
            "unit": "us/iter",
            "extra": "iterations: 60694\ncpu: 11.50438666095497 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/256",
            "value": 16.378050134857002,
            "unit": "us/iter",
            "extra": "iterations: 42266\ncpu: 16.37775128945261 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/4096",
            "value": 16515.281500005585,
            "unit": "us/iter",
            "extra": "iterations: 42\ncpu: 16513.848904761882 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/256",
            "value": 16.88058759885885,
            "unit": "us/iter",
            "extra": "iterations: 41222\ncpu: 16.87921260491943 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/4096",
            "value": 662.8788875235969,
            "unit": "us/iter",
            "extra": "iterations: 1058\ncpu: 662.8275727788242 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/256",
            "value": 23.174317761039934,
            "unit": "us/iter",
            "extra": "iterations: 30139\ncpu: 23.171787152858045 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/4096",
            "value": 1136.5177918035397,
            "unit": "us/iter",
            "extra": "iterations: 610\ncpu: 1136.443521311433 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/256",
            "value": 23.9126948107874,
            "unit": "us/iter",
            "extra": "iterations: 29041\ncpu: 23.911478668090076 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/4096",
            "value": 1133.4793030788262,
            "unit": "us/iter",
            "extra": "iterations: 617\ncpu: 1133.3613128039015 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/256",
            "value": 26.023603230590382,
            "unit": "us/iter",
            "extra": "iterations: 26930\ncpu: 26.021423431118013 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/4096",
            "value": 1312.2413780260597,
            "unit": "us/iter",
            "extra": "iterations: 537\ncpu: 1312.1538435753766 us\nthreads: 1"
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
        "date": 1752071920952,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/BN/ModAdd/256",
            "value": 0.06848245760547213,
            "unit": "us/iter",
            "extra": "iterations: 7968895\ncpu: 0.06847760988192216 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModAdd/4096",
            "value": 0.2768190411936343,
            "unit": "us/iter",
            "extra": "iterations: 2550901\ncpu: 0.27680174887226083 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/256",
            "value": 0.06098295373150912,
            "unit": "us/iter",
            "extra": "iterations: 11560008\ncpu: 0.06097693902980016 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/4096",
            "value": 0.3249427795796742,
            "unit": "us/iter",
            "extra": "iterations: 2156398\ncpu: 0.3249055132679549 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/256",
            "value": 0.15331388719457212,
            "unit": "us/iter",
            "extra": "iterations: 4534782\ncpu: 0.1533087449407705 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/4096",
            "value": 10.906605168810145,
            "unit": "us/iter",
            "extra": "iterations: 64154\ncpu: 10.905942388627277 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/256",
            "value": 16.36155238117737,
            "unit": "us/iter",
            "extra": "iterations: 42773\ncpu: 16.36074051387535 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/4096",
            "value": 16560.407523808775,
            "unit": "us/iter",
            "extra": "iterations: 42\ncpu: 16559.329000000183 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/256",
            "value": 17.39874947666931,
            "unit": "us/iter",
            "extra": "iterations: 40128\ncpu: 17.39622470594083 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/4096",
            "value": 669.7020596152906,
            "unit": "us/iter",
            "extra": "iterations: 1040\ncpu: 669.6890615384468 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/256",
            "value": 23.3805577536281,
            "unit": "us/iter",
            "extra": "iterations: 30734\ncpu: 23.379321728379104 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/4096",
            "value": 1197.5520119453236,
            "unit": "us/iter",
            "extra": "iterations: 586\ncpu: 1197.4507559726887 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/256",
            "value": 23.39718020312513,
            "unit": "us/iter",
            "extra": "iterations: 30621\ncpu: 23.39686261062656 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/4096",
            "value": 1220.4797302404234,
            "unit": "us/iter",
            "extra": "iterations: 582\ncpu: 1220.3773591065517 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/256",
            "value": 25.86405579414749,
            "unit": "us/iter",
            "extra": "iterations: 27476\ncpu: 25.863667637210863 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/4096",
            "value": 1374.4812291260864,
            "unit": "us/iter",
            "extra": "iterations: 515\ncpu: 1374.3463786408054 us\nthreads: 1"
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
        "date": 1752241155992,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/BN/ModAdd/256",
            "value": 0.0702559658790363,
            "unit": "us/iter",
            "extra": "iterations: 9513686\ncpu: 0.0702484696257581 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModAdd/4096",
            "value": 0.2688333178434044,
            "unit": "us/iter",
            "extra": "iterations: 2636119\ncpu: 0.2687975819756221 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/256",
            "value": 0.06298598008588259,
            "unit": "us/iter",
            "extra": "iterations: 11122964\ncpu: 0.06298203788126999 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/4096",
            "value": 0.3229547993786018,
            "unit": "us/iter",
            "extra": "iterations: 2166718\ncpu: 0.3229238562655643 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/256",
            "value": 0.14630793850107215,
            "unit": "us/iter",
            "extra": "iterations: 4728928\ncpu: 0.1463021902638395 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/4096",
            "value": 10.965661073244341,
            "unit": "us/iter",
            "extra": "iterations: 63825\ncpu: 10.965400846063542 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/256",
            "value": 16.35127011263445,
            "unit": "us/iter",
            "extra": "iterations: 42349\ncpu: 16.34939148504104 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/4096",
            "value": 16671.300452382333,
            "unit": "us/iter",
            "extra": "iterations: 42\ncpu: 16663.05490476207 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/256",
            "value": 17.48178463375925,
            "unit": "us/iter",
            "extra": "iterations: 40192\ncpu: 17.479663689291424 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/4096",
            "value": 667.9335542858531,
            "unit": "us/iter",
            "extra": "iterations: 1050\ncpu: 667.8126609523766 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/256",
            "value": 23.19486773475144,
            "unit": "us/iter",
            "extra": "iterations: 30522\ncpu: 23.193846209291934 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/4096",
            "value": 1167.5769102989111,
            "unit": "us/iter",
            "extra": "iterations: 602\ncpu: 1167.4887757475067 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/256",
            "value": 23.05608893552532,
            "unit": "us/iter",
            "extra": "iterations: 30494\ncpu: 23.05360956253685 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/4096",
            "value": 1176.1614133335265,
            "unit": "us/iter",
            "extra": "iterations: 600\ncpu: 1176.046391666669 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/256",
            "value": 25.296791622657416,
            "unit": "us/iter",
            "extra": "iterations: 27455\ncpu: 25.29322873793423 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/4096",
            "value": 1325.4823840302536,
            "unit": "us/iter",
            "extra": "iterations: 526\ncpu: 1325.2833631178744 us\nthreads: 1"
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
        "date": 1758812353326,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/BN/ModAdd/256",
            "value": 0.06890034869959083,
            "unit": "us/iter",
            "extra": "iterations: 10624618\ncpu: 0.06889634930874693 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModAdd/4096",
            "value": 0.27466258636185786,
            "unit": "us/iter",
            "extra": "iterations: 2539026\ncpu: 0.27464325453933847 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/256",
            "value": 0.061189163170651976,
            "unit": "us/iter",
            "extra": "iterations: 11495964\ncpu: 0.06118420473480949 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/4096",
            "value": 0.3228977479815173,
            "unit": "us/iter",
            "extra": "iterations: 2171341\ncpu: 0.32287301579991556 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/256",
            "value": 0.14570360325229845,
            "unit": "us/iter",
            "extra": "iterations: 4813457\ncpu: 0.1456875827497796 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/4096",
            "value": 11.010577963654542,
            "unit": "us/iter",
            "extra": "iterations: 63722\ncpu: 11.009918065974142 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/256",
            "value": 16.274176299773654,
            "unit": "us/iter",
            "extra": "iterations: 43046\ncpu: 16.27328248850075 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/4096",
            "value": 16512.720333328918,
            "unit": "us/iter",
            "extra": "iterations: 42\ncpu: 16512.439119047427 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/256",
            "value": 17.327703730237637,
            "unit": "us/iter",
            "extra": "iterations: 40480\ncpu: 17.326752742094946 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/4096",
            "value": 674.1498070176475,
            "unit": "us/iter",
            "extra": "iterations: 1026\ncpu: 674.1329142300136 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/256",
            "value": 23.417690781402797,
            "unit": "us/iter",
            "extra": "iterations: 29332\ncpu: 23.416039922269114 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/4096",
            "value": 1203.122156626273,
            "unit": "us/iter",
            "extra": "iterations: 581\ncpu: 1203.003753872632 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/256",
            "value": 24.06108179891073,
            "unit": "us/iter",
            "extra": "iterations: 30196\ncpu: 24.058597297655908 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/4096",
            "value": 1204.0028065628999,
            "unit": "us/iter",
            "extra": "iterations: 579\ncpu: 1203.9121174438767 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/256",
            "value": 26.621356417799603,
            "unit": "us/iter",
            "extra": "iterations: 27190\ncpu: 26.61959830820149 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/4096",
            "value": 1385.20700392924,
            "unit": "us/iter",
            "extra": "iterations: 509\ncpu: 1385.1353222003618 us\nthreads: 1"
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
        "date": 1759938065233,
        "tool": "googlecpp",
        "benches": [
          {
            "name": "Core/BN/ModAdd/256",
            "value": 0.06901656654318286,
            "unit": "us/iter",
            "extra": "iterations: 10564123\ncpu: 0.06881947872057151 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModAdd/4096",
            "value": 0.2758796892395787,
            "unit": "us/iter",
            "extra": "iterations: 2535459\ncpu: 0.2758518694248272 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/256",
            "value": 0.06108177479611619,
            "unit": "us/iter",
            "extra": "iterations: 11475504\ncpu: 0.06107219003191459 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModSubtract/4096",
            "value": 0.3230612420638416,
            "unit": "us/iter",
            "extra": "iterations: 2162272\ncpu: 0.32304132551316167 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/256",
            "value": 0.1447371309407257,
            "unit": "us/iter",
            "extra": "iterations: 4837028\ncpu: 0.14472224928199737 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModMultiply/4096",
            "value": 10.959584330556966,
            "unit": "us/iter",
            "extra": "iterations: 63844\ncpu: 10.958811618946196 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/256",
            "value": 16.300303100172645,
            "unit": "us/iter",
            "extra": "iterations: 42933\ncpu: 16.29881375631797 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModExponentiate/4096",
            "value": 16521.579999995258,
            "unit": "us/iter",
            "extra": "iterations: 42\ncpu: 16520.470714285788 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/256",
            "value": 17.395345487014026,
            "unit": "us/iter",
            "extra": "iterations: 40317\ncpu: 17.394435920331315 us\nthreads: 1"
          },
          {
            "name": "Core/BN/ModInvert/4096",
            "value": 677.8970465567467,
            "unit": "us/iter",
            "extra": "iterations: 1031\ncpu: 677.8636382153368 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/256",
            "value": 23.340485231259052,
            "unit": "us/iter",
            "extra": "iterations: 30876\ncpu: 23.33844364555037 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD/4096",
            "value": 1203.4815233159695,
            "unit": "us/iter",
            "extra": "iterations: 579\ncpu: 1203.365924006897 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/256",
            "value": 23.206319293798266,
            "unit": "us/iter",
            "extra": "iterations: 30699\ncpu: 23.205049056973113 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-RSA-Modulus/4096",
            "value": 1201.384780068617,
            "unit": "us/iter",
            "extra": "iterations: 582\ncpu: 1201.2929089347044 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/256",
            "value": 25.782387807519207,
            "unit": "us/iter",
            "extra": "iterations: 27640\ncpu: 25.78097073082492 us\nthreads: 1"
          },
          {
            "name": "Core/BN/GCD-Batch(16)RSA-Modulus/4096",
            "value": 1379.1480137249034,
            "unit": "us/iter",
            "extra": "iterations: 510\ncpu: 1379.0272686274345 us\nthreads: 1"
          }
        ]
      }
    ]
  }
}