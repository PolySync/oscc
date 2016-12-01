#!groovy
node('worker') {
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

    }, 'brake firmware': {

    }, 'CAN gateway firmware': {

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