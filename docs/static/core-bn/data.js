window.BENCHMARK_DATA = {
  "lastUpdate": 1746512423765,
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
      }
    ]
  }
}