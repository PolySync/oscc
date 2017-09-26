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
      parallel 'kia soul petrol firmware': {
        sh 'cd firmware && mkdir build_kia_soul_petrol && cd build_kia_soul_petrol && cmake .. -DKIA_SOUL=ON -DCMAKE_BUILD_TYPE=Release && make'
      }, 'kia soul EV firmware': {
        sh 'cd firmware && mkdir build_kia_soul_ev && cd build_kia_soul_ev && cmake .. -DKIA_SOUL_EV=ON -DCMAKE_BUILD_TYPE=Release && make'
      }
      echo 'Build Complete!'
    }
    stage('Kia Soul Petrol Tests') {
      parallel 'kia soul petrol firmware unit tests': {
        sh 'cd firmware && mkdir build_kia_soul_petrol_unit_tests && cd build_kia_soul_petrol_unit_tests && cmake .. -DKIA_SOUL=ON -DTESTS=ON -DCMAKE_BUILD_TYPE=Release && make run-unit-tests'
        echo 'Kia Soul Petrol Firmware Unit Tests Complete!'
      }, 'kia soul petrol firmware property-based tests': {
        sh 'cd firmware && mkdir build_kia_soul_petrol_property_tests && cd build_kia_soul_petrol_property_tests && cmake .. -DKIA_SOUL=ON -DTESTS=ON -DCMAKE_BUILD_TYPE=Release && make run-property-tests'
        echo 'Kia Soul Petrol Firmware Property-Based Tests Complete!'
      }, 'kia soul petrol api property-based tests': {
        sh '''
          ls -la ~
          ls -la ~/.cargo/bin
          which rustc
          which cargo
          rustc --version
          cargo --version
          sudo ip link set vcan0 down && sudo rmmod vcan
          cd api/tests && chmod +x initialize_vcan.sh && ./initialize_vcan.sh
          mkdir build_kia_soul_petrol_property_tests
          cd build_kia_soul_petrol_property_tests
          cmake .. -DKIA_SOUL=ON
          make run-api-property-tests
          sudo ip link set vcan0 down && sudo rmmod vcan
        '''
        echo 'Kia Soul Petrol API Property-Based Tests Complete!'
      }
      echo 'Kia Soul Petrol Tests Complete!'
    }
    stage('Kia Soul EV Tests') {
      parallel 'kia soul ev unit tests': {
        sh 'cd firmware && mkdir build_kia_soul_ev_unit_tests && cd build_kia_soul_ev_unit_tests && cmake .. -DKIA_SOUL_EV=ON -DTESTS=ON -DCMAKE_BUILD_TYPE=Release && make run-unit-tests'
        echo 'Kia Soul EV Firmware Unit Tests Complete!'
      }, 'kia soul ev property-based tests': {
        sh 'cd firmware && mkdir build_kia_soul_ev_property_tests && cd build_kia_soul_ev_property_tests && cmake .. -DKIA_SOUL_EV=ON -DTESTS=ON -DCMAKE_BUILD_TYPE=Release && make run-property-tests'
        echo 'Kia Soul EV Firmware Property-Based Tests Complete!'
      }, 'kia soul ev api property-based tests': {
        sh '''
          ls -la ~
          ls -la ~/.cargo/bin
          which rustc
          which cargo
          rustc --version
          cargo --version
          sudo ip link set vcan0 down && sudo rmmod vcan
          cd api/tests && chmod +x initialize_vcan.sh && ./initialize_vcan.sh
          mkdir build_kia_soul_ev_property_tests
          cd build_kia_soul_ev_property_tests
          cmake .. -DKIA_SOUL_EV=ON
          make run-api-property-tests
          sudo ip link set vcan0 down && sudo rmmod vcan
        '''
        echo 'Kia Soul EV API Property-Based Tests Complete!'
      }
      echo 'Kia Soul EV Tests Complete!'
    }
  }
  catch(Exception e) {
    throw e;
  }
  finally {
    deleteDir()
  }
}
