{
  "targets": [
    {
      "target_name": "_nd-geohash-core",
      "sources": [
        "./src/geo.cc",
        "./src/geometry/s1angle.cc",
        "./src/geometry/s1interval.cc",
        "./src/geometry/s2.cc",
        "./src/geometry/s2cap.cc",
        "./src/geometry/s2cell.cc",
        "./src/geometry/s2cellid.cc",
        "./src/geometry/s2cellunion.cc",
        "./src/geometry/s2edgeindex.cc",
        "./src/geometry/s2edgeutil.cc",
        "./src/geometry/s2latlng.cc",
        "./src/geometry/s2latlngrect.cc",
        "./src/geometry/s2loop.cc",
        "./src/geometry/s2pointregion.cc",
        "./src/geometry/s2polygon.cc",
        "./src/geometry/s2polygonbuilder.cc",
        "./src/geometry/s2polyline.cc",
        "./src/geometry/s2r2rect.cc",
        "./src/geometry/s2region.cc",
        "./src/geometry/s2regioncoverer.cc",
        "./src/geometry/s2regionintersection.cc",
        "./src/geometry/s2regionunion.cc",
        "./src/geometry/strings/strutil.cc",
        "./src/geometry/strings/stringprintf.cc",
        "./src/geometry/util/math/exactfloat/exactfloat.cc"
      ],
      "defines": [
        'NDEBUG'
      ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
        "./src/geometry/",
        "./src/geometry/base/",
        "./src/geometry/s2/",
        "./src/geometry/strings/",
        "./src/geometry/util/",
        "./src/geometry/util/math/",
        "<(node_root_dir)/deps/openssl/openssl/include"
      ],
      'conditions': [
        ["target_arch=='ia32'", {
          "include_dirs": [ "<(node_root_dir)/deps/openssl/config/piii" ]
        }],
        ["target_arch=='x64'", {
          "include_dirs": [ "<(node_root_dir)/deps/openssl/config/k8" ]
        }],
        ["target_arch=='arm'", {
          "include_dirs": [ "<(node_root_dir)/deps/openssl/config/arm" ]
        }],
        ['OS=="mac"', {
          'xcode_settings': {
            'OTHER_CPLUSPLUSFLAGS':[
              '-Wno-deprecated',
              '-Wno-ignored-qualifiers',
              '-DARCH_K8 -DS2_USE_EXACTFLOAT',
              '-stdlib=libc++',
              '-std=c++11'
            ],
            'OTHER_LDFLAGS':[
              '-flat_namespace -undefined suppress',
              '-stdlib=libc++',
              '-std=c++11'],
            'GCC_ENABLE_CPP_RTTI': 'YES',
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
            'MACOSX_DEPLOYMENT_TARGET': '10.7',
          }
        }],
        ['OS=="solaris"',{
          'cflags_cc' : [
            '-Wno-deprecated',
            '-Wno-ignored-qualifiers',
            '-DARCH_K8 -DS2_USE_EXACTFLOAT',
            '-std=c++11'
          ]
        }],
        ['OS=="linux"', {
          'cflags_cc' : [
            '-Wno-deprecated',
            '-Wno-ignored-qualifiers',
            '-DARCH_K8 -DS2_USE_EXACTFLOAT',
            '-std=c++11'
          ],
        }]
      ]
    }
  ]
}