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
        sh 'cd firmware && mkdir build && cd build && cmake .. -DKIA_SOUL=ON -DCMAKE_BUILD_TYPE=Release && make'
      }, 'joystick commander': {
        sh 'cd applications/joystick_commander && mkdir build && cd build && cmake .. -DKIA_SOUL=ON && make'
      }, 'diagnostics tool': {
        sh 'cd applications/diagnostics_tool && mkdir build && cd build && cmake .. && make'
      }
      echo 'Build Complete!'
    }
    stage('Test') {
      parallel 'unit tests': {
        sh 'cd firmware && mkdir build_unit_tests && cd build_unit_tests && cmake .. -DKIA_SOUL=ON -DTESTS=ON -DCMAKE_BUILD_TYPE=Release && make run-unit-tests'
        echo 'Unit Tests Complete!'
      }, 'property-based tests': {
        sh 'cd firmware && mkdir build_property_tests && cd build_property_tests && cmake .. -DKIA_SOUL=ON -DTESTS=ON -DCMAKE_BUILD_TYPE=Release && make run-property-tests'
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
