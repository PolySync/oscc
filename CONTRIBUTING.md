# How to contribute

There are many ways to contribute to the OSCC project; support for additional vehicles can be added, diagrams can be made to look better, car systems can be better described,
CAN frames can be better detailed, PCBs can be optimized
or code could be refactored and improved.

The directory structure of the project is created in such a way that adding support for additional vehicles should be simple and intuitive. All vehicle specific directories should be reacreated for each additional vehicle. Below is a sample of how additional vehicle directories should be created.


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
  * Clearly describe the issue including steps to reproduce when it is a bug.
* Fork the repository on GitHub

## Making Changes

* Create a topic branch from where you want to base your work.
  * You'll likely branch off of the master branch.
  * To quickly create a topic branch based on master; `git checkout -b
    fix/master/my_contribution master`. Please avoid working directly on the
    `master` branch.
* Write a good commit message.

## Submitting Changes

* Push your changes to a topic branch in your fork of the repository.
* Submit a pull request to the repository in the PolySync organization.
* Update your github issue to mark that you have submitted code and are ready for it to be reviewed (Status: Ready for Merge).
  * Include a link to the pull request in the ticket.
* The PolySync team will review all pull requests as they come up.
