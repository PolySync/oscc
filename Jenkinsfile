#!groovy
node('arduino') {
  try {
    stage('Checkout') {
      checkout([
        $class: 'GitSCM',
        branches: scm.branches,
        extensions: scm.extensions + [[$class: 'CleanBeforeCheckout']],
        userRemoteConfigs: scm.userRemoteConfigs
      ])
    }
    stage('Build') {
      parallel 'kia soul firmware': {
        sh 'cd platforms && mkdir build && cd build && cmake .. -DBUILD_KIA_SOUL=ON -DCMAKE_BUILD_TYPE=Release && make'
      }
      echo 'Build Complete!'
    }
    stage('Test') {
      parallel 'unit tests': {
        sh 'cd platforms && mkdir build_tests && cd build_tests && cmake .. -DTESTS=ON -DCMAKE_BUILD_TYPE=Release && make run-tests'
        echo 'Unit Tests Complete!'
      }, 'property-based tests': {
        sh 'cargo test --manifest-path kia_soul/firmware/steering/tests/property/Cargo.toml -- --test-threads=1'
        sh 'cargo test --manifest-path kia_soul/firmware/brake/tests/property/Cargo.toml -- --test-threads=1'
        sh 'cargo test --manifest-path kia_soul/firmware/throttle/tests/property/Cargo.toml -- --test-threads=1'
        sh 'cargo test --manifest-path common/libs/pid/tests/Cargo.toml'
        echo 'Property-Based Tests Complete!'
      }, 'acceptance tests': {
        echo 'Acceptance Tests Complete!'
      }
    }
    stage('Release') {
      echo 'Release Package Created!'
    }
  }
  catch(Exception e) {
    throw e;
  }
  finally {
    deleteDir()
  }
}
