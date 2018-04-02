#!groovy

def platforms = ['kia_soul_petrol', 'kia_soul_ev', 'kia_niro']

node('worker'){
  def builds = [:]
  for(int j=0; j<platforms.size(); j++) {
    def platform_idx = j
    def platform = platforms[platform_idx]
    builds[platform] = {
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
            sh 'cd firmware && mkdir build_${platform} && cd build_${platform} && cmake -D${platform.toUpperCase()}=ON -DCMAKE_BUILD_TYPE=Release .. && make'
            echo '${platform}: Build Complete!'
          }
          stage('Test ${platform} unit tests') {
            sh 'cd firmware && mkdir build_${platform}_unit_tests && cd build_${platform}_unit_tests && cmake -D${platform.toUpperCase()} -DCMAKE_BUILD_TYPE=Release .. && make run-unit-tests'
            echo '${platform}: Unit Tests Complete!'
          }
          stage('Test ${platform} property-based tests') {
              sh 'cd firmware && mkdir build_${platform}_property_tests && cd build_${platform}_property_tests && cmake -D${platform.toUpperCase()}=ON -DTESTS=ON -DCMAKE_BUILD_TYPE=Release .. && make run-property-tests'
              echo '${platform}: Property-Based Tests Complete!'
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
