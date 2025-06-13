window.BENCHMARK_DATA = {
  "lastUpdate": 1749827754806,
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
      }
    ]
  }
}