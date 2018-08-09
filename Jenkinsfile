#!groovy

node {
    checkout scm

    def image = docker.build("cmake-build:${env.BUILD_ID}")

    def builds = [:]
    def output = image.inside {
        sh returnStdout: true, script: "cmake -LA ./firmware | grep 'VEHICLE_VALUES' | cut -d'=' -f 2"
    }

    def platforms = output.trim().tokenize(';')

    for(int j=0; j<platforms.size(); j++) {
        def platform_idx = j
        def platform = platforms[platform_idx]
        builds[platform] = {
            node {
                try {
                    checkout scm
                    
                    image = docker.build("cmake-build:${env.BUILD_ID}")

                    stage("Build ${platform}") {
                        image.inside {
                            sh "cd firmware && \
                                rm -rf build_${platform} && \
                                mkdir build_${platform} && \
                                cd build_${platform} && \
                                cmake -DVEHICLE=${platform} -DCMAKE_BUILD_TYPE=Release .. && \
                                make"

                            echo "${platform}: Build Complete!"
                        }
                    }

                    stage("Test ${platform} unit tests") {
                        image.inside {
                            sh "cd firmware && \
                                rm -rf build_${platform}_tests && \
                                mkdir build_${platform}_tests && \
                                cd build_${platform}_tests && \
                                cmake -DVEHICLE=${platform} \
                                  -DTESTS=ON \
                                  -DPORT_SUFFIX=${EXECUTOR_NUMBER}${platform_idx} \
                                  -DCMAKE_BUILD_TYPE=Release \
                                  .. && \
                                make run-unit-tests"
                            echo "${platform}: Unit Tests Complete!"
                        }
                    }

                    stage("Test ${platform} property-based tests") {
                        image.inside("--user root:root") {
                            sh "cd firmware/build_${platform}_tests && \
                                make run-property-tests"
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
