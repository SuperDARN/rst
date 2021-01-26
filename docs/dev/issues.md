<!---
(C) copyright 2021 SuperDARN Canada, University of Saskatchewan 
author: Marina Schmidt, SuperDARN Canada

Modifications:

RST is a free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
-->

# Issues 

Issues on GitHub are a convenient way to notify DAWG members and other people about the problem/update at hand. 
Issue are not just for bug reporting, other options are:

- New Feature announcement 
- Deprecation of code announcement 
- Discussion on something in RST
- Question regarding installation or general use of RST 

In RST, these above categories of issue are templated to help anyone fill in the correct information to get insightful help.

## Other Working Group Issues

Before getting started please ensure this issue pertains to RST. Here are other working groups in SuperDARN that may be better at answering your issue:

- [DDWG](https://github.com/SuperDARN/DDWG): Data Distribution Working Group, focuses on checking and distributing files. If you think your file is corrupt, or incorrect content wise starting at RAWACF level, then this file may need to be block listed. Create an issue there. 
- [DSWG](https://github.com/SuperDARN/dswg): Data Standard Working Group, focuses on standards and standard formats in SuperDARN community. If you are:
    - purposing a new format
    - change to a format
    - questioning a standard
    - wondering about metadata - hardware files
  Then create an new issue there. 
- [SWG](https://github.com/SuperDARN/schedules): Scheduling Working Group, focuses on scheduling discretionary monthly times and documenting control program IDs. If you are wondering about control program IDs or schedule, then create an issue there.  
- [DAWG](https://github.com/SuperDARN/dawg): Data Analysis Working Group, focuses on processing and analysis tools from FITACF and higher data products. If you have a question for general DAWG operations, new software support, or charter and general guidelines, then create an issue there. 

## Getting Started 

To make an issue go to the [RST repository](https://github.com/SuperDARN/rst)

1. Go to [issues page](https://github.com/SuperDARN/rst/issues) top left side. 
2. Then look at the currently open issues to see if you issue is already open. Also, you can use the search bar at the top of the list to find similar issues. 
3. If the issue is not create then click on [New issue](https://github.com/SuperDARN/rst/issues/new/choose) button top right
4. Select one of the appropriate issue templates or select *open a blank issue* at the bottom of the page if non of the template are appropriate for issue. 
5. Fill out the template  information to provide enough information for insightful help from others. If you are using a blank issue please provide an appropriate:
    - descriptive title 
    - Scope: what is this issue focused on
    - provide extra information like: code, expect output, computer specs, questions, and/or concerns. 
* Fill out the side bar information for better awareness on the issue:  
    - Assignees: this informs anyone the person(s) taking on dealing with the issue. This will inform other to not work on the issue. 
    - Lables: categorizes issues for certain members with specific expertise can help you. You can have multiple labels. 
    - Projects: organizes issues into tasks for given projects, see [projects](projects.md) 
    - Milestone: indicates what release this issue should be solved by. Please do not fill out. Leaders, Chairs, or senior developers can fill this out by understanding the importance and timeline for this issue. 
    - Linked Pull Request: this links pull requests to your issues to inform users the issue has been resolved in a pull request. This should be filled out when a pull requests is created, see [pull requests](pull_request.md)
* Click *Submit new issue*, this will send an email to anyone who is subscribed to the repository. Depending on your notification settings for GitHub you should receive an email on the issue and the on going conversation. If not look into your settings as a user to see what you are subscribed to and notification settings. 



## Following The Issue

It is important you follow the on going conversation of the issue if DAWG members need more information.
If you do not respond for a while, the issue will go **stale** leading to a potential closure of your issue. However, DAWG tries to keep issues open and contact users on their issues if they have gone stale. 

Please remember, DAWG members are trying to get all the information from you, so respect that may need a lot more information then you provided so you may need to follow up frequently. 
Please also remember, DAWG members live in various parts around the world and may have other responsibilities so be patient on getting a response. 
As a rule of thumb if a week has passed with no response and you need one sooner than later, please send out an ask in the conversation if someone can look at the issue. This will prevent the issue 
going **stale** and a small request on help will notify everyone again. Don't be afraid to do this. 

Likewise DAWG members may notify you to get a response so please be diligent in following up. 

## Stale Issues

If an issue goes stale (no progress for over 3 months) then a DAWG member should follow the following guidelines:

1. Ask for other input or status of the issue in the conversation
2. Contact the user via email 
3. Request if we should discuss it in the next DAWG meeting (add label *telecon*) or placed in a project to be re-opened later?
* Label the issue **stale** until progress can be made on it again 
* Request to close it if non of the above steps warrant any response or information to keep it open
* Leaders and Chairs of the software package/DAWG can approve or close **stale** issues 

## Assignees

Assignees section of an issue is useful to ensure someone is looking into the issue and taking the responsibility for it. This avoids multiple people addressing the same issue.
If someone has assigned themselves to the issue; However, minimal progress has been made, please contact them on this and request to change the assignee if you want to take it on for faster progress. 

## Labels 

RST supports the following labels:

| Label                | Definition                                               |
| -------------------- | -------------------------------------------------------- |
| bug                  | Issue is about a potential bug                           |
| discussion           | Issue requires a discussion                              |
| documentation        | Issue pertains to documentation                          |
| DSWG                 | This is DSWG issue                                       |
| duplicate            | There is already another issue about the same problem    |
| enhancement          | Issue pertains about enhancing RST                       |
| Error/Warning Checks | Issue is purposing an new error or warning check for RST |
| help wanted          | Issue is looking for some help on the problem            |
| high priority        | Issue needs to be looked at over others                  |
| invalid              | Issue about something being invalid                      |
| low priority         | Issue is not very important at the time                  |
| PI input             | Issue requires PI input                                  |
| Programmer needed    | Issue needs programmers help                             |
| question             | Issue regards to a question                              |
| Scientist input      | Issue needs Scientist help                               |
| telecon              | Issue needs to be discussed in a telecon                 |
| wontfix              | Issue could not be fixed                                 |
| workaround           | Issue was fixed with a workaround                        | 


### Creating A New Label

To create an new label got [RST labels](https://github.com/SuperDARN/rst/labels) and click on [New label](https://github.com/SuperDARN/rst/labels)

Give a concise name for the label then a description on what this label is reference to. A color can be randomly generated or chosen based on the hex value. Please avoid similar colors to other labels if possible. 

Then click on *Create Label*.

Please note to do not create a new label while creating a new issue as this can delete the issue being created. 

## Open ended Issues 

Sometimes Issues can be open ended especially discussion issues. Here are some steps to make progress on the issue and prevent it going **stale**:
- Label it with *telecon* if needs to be further discussed in a telecon with DAWG members 
- Once a general idea is established, close the issue and create a project to break into a defined tasks. Open the first task as an issue, see [projects](projects.md).



