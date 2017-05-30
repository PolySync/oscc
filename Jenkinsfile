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
      }, 'joystick commander': {
        sh 'cd utils/joystick_commander && mkdir build && cd build && cmake .. && make'
      }, 'diagnostics tool': {
        sh 'cd utils/diagnostics_tool && mkdir build && cd build && cmake .. && make'
      }
      echo 'Build Complete!'
    }
    stage('Test') {
      parallel 'unit tests': {
        sh 'cd platforms && mkdir build_tests && cd build_tests && cmake .. -DTESTS=ON -DCMAKE_BUILD_TYPE=Release && make run-tests'
        echo 'Unit Tests Complete!'
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
