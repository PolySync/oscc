# How to contribute

There are many ways to contribute to the OSCC project; support for additional vehicles can be added, diagrams can be made to look better, car systems can be better described,
CAN frames can be better detailed, PCBs can be optimized
or code could be refactored and improved.

The directory structure of the project is created in such a way that adding support for additional vehicles should be simple and intuitive.
All vehicle specific directories should be recreated for each additional vehicle. The naming convention for is <make>_<model>_<generational identifier>
Below is a sample of how additional vehicle directories should be created.


    .
    ├── firmware
    │   ├── ...
    │   ├── brake
    │   │   ├──kia_soul_ps
    │   │   ├──<my_new_car>       # Brake firmware for vehicle you're adding
    │   ├── ...
    ├── vehicle_info
    │   ├──kia_soul_ps
    │   ├──<my_new_car>           # Vehicle specific information for vehicle you're adding
    └── ...


## Getting Started

* Using the repo's Issues section, create a new issue and:
  * Clearly describe the issue including steps to reproduce when it is a bug
  * Branch the repository on GitHub

## Making Contributions

* Do not work directly on the `master` branch; changes to the master branch will be removed
* Create a topic branch from where you want to base your work
  * You'll likely branch off of the master branch
  * To quickly create a topic branch based on master:
  ```
  git checkout -b fix/master/my_contribution master
  ```

* Write a good commit message.
#### Commit Messages
* Use the present tense ("Add feature" not "Added feature")
* Use the imperative mood ("Move resistor to..." not "Moves resistor to...")
* Limit the first line to 72 characters or less
* Reference issues and pull requests specifically
* When changing documentation only, include [ci skip] in the commit description

### Contributing Code

* Code should conform to the coding standard (OSCC-CODING.md)
* Push your changes to a topic branch in your branch of the repository
* Submit a pull request to the repository in the PolySync organization
* Update your github issue to mark that you have submitted code and are ready for it to be reviewed (Status: Ready for Merge)
  * Include a link to the pull request in the ticket
* The PolySync team will review all pull requests on a weekly basis
* __Code contributed should include unit tests__ that demonstrate the code functions as expected

### Contributing a Diagram

* The diagrams were created using the Eagle PCB Design tool (https://cadsoft.io/#)
    * All board diagrams should use this tool
* Modifications should include tests to demonstrate the the modified board works as expected

### Submitting Enhancement Requests

Enhancement suggestions are tracked as GitHub issues:

* Use a clear and descriptive title for the issue to identify the suggestion
* Provide a step-by-step description of the suggested enhancement in as many details as possible
* Provide specific examples to demonstrate the steps
    * Include snippets in the examples as Markdown code blocks ( using the \`\`\` format )
* Describe the current behavior and how this enhancement improves or changes it
* Explain why this enhancement is useful to most users
* Specify which version of OSCC this change applies to
    * The versions can be found in the Releases tab on GitHub

### Pull Request Labels
* Fill in the required template
* Follow the style guides
* Code should be complete
    * Any changes should include unit tests for the submitted
* End files with a new line
* Changes should be located in the most appropriate directory
* Please open an issue to suggest a new label.

## Directory Contents

This information is also available in the README.

| Directory | Contents |
| ---------:|:-------- |
| 3d_tools | Technical drawings and 3D files for board enclosures and other useful parts |
| assets | Diagrams and images used in the wiki |
| boards | PCB schematics and board designs for control modules that are integrated with the vehicle, typically these are the throttle, steering and brake boards, but can include other items should they be contributed. |
| control | User Space applications to provide mechanisms for the user to control the vehicle directly via something like a PC.  Applications can be command-line or GUI based and include items like a joystick controller |
| firmware | Arduino code for the throttle, steering and brake functionality.  This directory also includes code for the CAN gateway module, which uses a stock Arduino board. The firmware should remain modular and portable to a a different Arduino board |
| vehicle_info | Information on specific vehicles such as sensor outputs and wiring diagrams.  Each additional vehicle should be in its own subdirectory |

## Diagrams

Adding diagrams for new boards or other components should be located in the directory most appropriate for their function:
* A new enclosure should go in the 3d_models directory
* A new board diagram should go in the boards directory
* Any new vehicle information should reside in the vehicle_info directory

### Editing diagrams

Existing diagrams should be edited using the Autodesk Eagle as mentioned above.

## Building

To build the various components in the system, please see the README.md

## Documentation

* Documentation updates are welcome
* Documentation should be located in the *doc* directory where it is most applicable (See the Directory descriptions above)
	* If the *doc* directory does not exist, please create it as part of the submission
* Documentation pull requests function in the same way as other pull requests

### Documentation Style

* Documentation should be in markdown format

## Helpful Links

* [Autodesk Eagle](https://cadsoft.io)
* [Arduino FAQ](https://www.arduino.cc/en/Main/FAQ)
* [Throttle/Steering/CAN Gateway board](https://www.arduino.cc/en/Main/ArduinoBoardUno)
* [Braking board](https://www.arduino.cc/en/Main/arduinoBoardMega2560)
