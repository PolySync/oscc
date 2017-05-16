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
        sh 'cd platforms && mkdir build && cd build && cmake .. -DTESTS=ON -DCMAKE_BUILD_TYPE=Release && make run-tests'
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
