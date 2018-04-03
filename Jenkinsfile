#!groovy

def cleanCheckout() {
  checkout([
    $class: 'GitSCM',
    branches: scm.branches,
    extensions: scm.extensions + [[$class: 'CleanBeforeCheckout']],
    userRemoteConfigs: scm.userRemoteConfigs
  ])
}

node('worker'){
  def builds = [:]
  cleanCheckout()
  def output = sh returnStdout: true, script: "cmake -LA ./firmware | grep 'VEHICLE_VALUES' | cut -d'=' -f 2"
  def platforms = output.trim().tokenize(';')

  for(int j=0; j<platforms.size(); j++) {
    def platform_idx = j
    def platform = platforms[platform_idx]
    builds[platform] = {
      node('arduino') {
        try {
          stage("Checkout") {
            cleanCheckout()
          }
          stage("Build ${platform}") {
            sh "cd firmware && mkdir build_${platform} && cd build_${platform} \
            && cmake -DVEHICLE=${platform} -DCMAKE_BUILD_TYPE=Release .. && make"
            echo "${platform}: Build Complete!"
          }
          stage("Test ${platform} unit tests") {
            sh "cd firmware && mkdir build_${platform}_tests && \
            cd build_${platform}_tests && cmake -DVEHICLE=${platform} \
            -DTESTS=ON -DPORT_SUFFIX=${EXECUTOR_NUMBER}${platform_idx} \
            -DCMAKE_BUILD_TYPE=Release .. && make run-unit-tests"
            echo "${platform}: Unit Tests Complete!"
          }
          stage("Test ${platform} property-based tests") {
            withEnv(["PATH+CARGO=$HOME/.cargo/bin"]) {
              sh "cd firmware/build_${platform}_tests && make run-property-tests"
              echo "${platform}: Property-Based Tests Complete!"
            }
          }
        }
        finally {
          deleteDir()
        }
      }
    }
  }

  try {
    parallel builds
  }
  finally {
    deleteDir()
  }

}
