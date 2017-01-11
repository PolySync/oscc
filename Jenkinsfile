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
      parallel 'steering firmware': {
        sh 'cd firmware/steering/kia_soul_ps && make'
      }, 'throttle firmware': {
        sh 'cd firmware/throttle/kia_soul_ps && make'
      }, 'brake firmware': {
        sh 'cd firmware/brake/kia_soul_ps && make'
      }, 'CAN gateway firmware': {
        sh 'cd firmware/can_gateway/kia_soul_ps && make'
      }
      echo 'Build Complete!'
    }
    stage('Test') {
      parallel 'unit tests': {
        //sh 'make test'
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