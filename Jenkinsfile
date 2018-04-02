#!groovy

def platforms = ['kia_soul_petrol', 'kia_soul_ev', 'kia_niro']

for(int j=0; j<platforms.size(); j++) {
  def platform_idx = j
  def platform = platforms[platform_idx]
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
      stage('Test ${platform}') {
        parallel '${platform} unit tests': {
          sh 'cd firmware && mkdir build_${platform}_unit_tests && cd build_${platform}_unit_tests && cmake -D${platform.toUpperCase()} -DCMAKE_BUILD_TYPE=Release .. && make run-unit-tests'
          echo '${platform}: Unit Tests Complete!'
        }, '${platform} property-based tests': {
          sh 'cd firmware && mkdir build_${platform}_property_tests && cd build_${platform}_property_tests && cmake -D${platform.toUpperCase()}=ON -DTESTS=ON -DCMAKE_BUILD_TYPE=Release .. && make run-property-tests'
          echo '${platform}: Property-Based Tests Complete!'
        }
      }
    }
    finally {
      deleteDir()
    }
  }
}
