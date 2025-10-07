# Discovr-GroupNumber
Discovr – Lightweight Asset Discovery Tool

Developed by La Trobe University students in collaboration with Triskele Labs as part of the 2024 Capstone Project.
Discovr identifies IT assets across on-premises and hybrid-cloud environments (AWS & Azure) to improve visibility and reduce security blind spots.

⚙️ Setup / Installation
Requirements

CMake ≥ 3.20

Ninja build system

LLVM (Clang / Clang ++)

Vim (for xxd utility)

Windows 10/11 or Linux (Ubuntu 22.04 LTS+)

Initial Setup
git clone https://github.com/<team-name>/Discovr-GroupNumber.git
cd Discovr-GroupNumber

Building (Windows or Linux)

Using VS Code with CMake Tools extension:

CMake → Configure (select preset developer-tools)

CMake → Build

CMake → Run Without Debugging

⚠️ Avoid the default “Run” button – it bypasses configured compilers and may cause build errors.

Manual Linux build:

mkdir build && cd build
cmake .. --preset linux-developer-tools
cmake --build .
./Discovr-GroupNumber-Linux

🧰 Why Embed Nmap

Traditional network-discovery tools require a separate Nmap installation and administrative setup.
Discovr embeds Nmap directly inside its binary using CMake’s FetchContent feature, creating a self-contained scanning engine.

Benefits

✅ Zero external dependencies – users don’t need to install Nmap.

✅ Cross-platform portability – the embedded engine works on Windows and Linux.

✅ Consistent results – always runs with a tested, pinned version of Nmap.

✅ Faster deployment – ideal for SMEs or restricted environments.

By embedding Nmap, Discovr achieves its goal of being a lightweight, portable asset-discovery solution capable of active and passive scans without external setup.

🖥️ Usage ( CLI Menu )

Run the built binary:

./Discovr-GroupNumber-Win      # Windows  
./Discovr-GroupNumber-Linux    # Linux

Main Menu
Discovr – Asset Discovery
=========================
1) Run Discovery  
2) Export Results  
3) View Settings  
4) Help / About  
5) Exit

Discovery Sub-menu
>> Discovery Options  
  a) Full Scan (active + passive + cloud)  
  b) Active Scan (Nmap-based)  
  c) Passive Scan (low-intensity)  
  d) Cloud Scan  
  e) Back to Main Menu

Cloud Sub-menu
>> Cloud Providers  
  1) AWS (uses Discover-AwsVms.ps1)  
  2) Azure (uses Discover-AzureVms.ps1)  
  3) Back to Discovery Menu

Export Sub-menu
>> Export Options  
  a) Export CSV  
  b) Export JSON  
  c) Back to Main Menu

📄 CSV Output Schema

Discovr standardizes CSV reporting for on-prem, AWS, and Azure assets.

Field	Description	Example
provider	Source of discovery (aws,azure,onprem)	aws
accountOrSub	AWS Account ID / Azure Subscription / Lab ID	147795258967
regionOrLocation	Region or Location of asset	ap-southeast-2
vmId	Unique instance / host / container ID	i-0187f1e9e4820a4d6
name	Host or VM name	Test-VM
state	Power or runtime state	running
size	Instance type / container size	t3.micro
os	Operating system family / version	Linux/UNIX
privateIp	Internal IP address	172.31.29.33
publicIp	Public IP (if available)	3.25.163.66
az	Availability zone (AWS only)	ap-southeast-2c
discoveredAt	UTC timestamp (ISO-8601)	2025-09-09T06:43:09Z

Sample Output

provider,accountOrSub,regionOrLocation,vmId,name,state,size,os,privateIp,publicIp,az,discoveredAt
aws,147795258967,ap-southeast-2,i-0187f1e9e4820a4d6,Test-VM,running,t3.micro,Linux/UNIX,172.31.29.33,3.25.163.66,ap-southeast-2c,2025-09-09T06:43:09Z
azure,abcd1234-ef56-7890-gh12-ijkl3456mnop,australiaeast,/subscriptions/abcd1234/resourceGroups/rg-demo/providers/Microsoft.Compute/virtualMachines/Test-Azure-VM,Test-Azure-VM,VM running,Standard_B2s,Windows,10.1.0.5,20.40.60.80,,2025-09-09T12:00:00Z
onprem,lab001,lab-net-172.30.0.0/16,container-web-01,nginx-01,running,container,Linux,172.30.0.10,,,2025-09-14T17:50:00Z

☁️ Cloud Discovery Mapping (ECS-37 by Jagsir Chugha)

Objective – Design and test cloud asset discovery for AWS and Azure.

Methodology

Authentication

AWS → aws configure (default CLI profile)

Azure → az login and az account set

Enumeration

AWS → aws ec2 describe-instances

Azure → az vm list -d

Normalization
Common schema aligned with Discovr’s CSV format.

Export
Separate: assets_aws.csv, assets_azure.csv
Unified: assets_cloud.csv

Findings

✅ AWS discovery successful (returned valid VM data).

⚠️ Azure blocked due to permissions; mock mode added for testing.

Challenges

Execution-policy restrictions on PowerShell scripts.

Azure AuthorizationFailed errors.

Excel showing Account IDs in scientific notation.


🧰 Error Handling & Logs

Runtime logging records scan events and errors in /logs/:

[2025-10-07 19:20:32] INFO – Azure discovery completed (3 VMs found)  
[2025-10-07 19:20:34] ERROR – Invalid AWS credentials detected

🧪 Testing & Validation

✅ Windows and Linux builds validated

✅ CLI navigation tested

✅ CSV export verified with AWS data

⚠️ Azure mock validated (pending live access)

🧾 Cross-platform logging verified

🧑‍💻 Development Notes
File	Purpose
CMakeLists.txt	Build automation and embedded Nmap integration
DevREADME.md	Developer setup guide
src/extractors/Extractor.cpp	Processes and exports discovery results
src/Discovr-GroupNumber.cpp	CLI logic and entry point
🧾 Known Issues / Future Work

Implement live Azure discovery

Add HTML export feature

Strengthen runtime logging and CLI error handling

Package portable binary for final release 

📚 Team & Roles
Name	Role

Zachary Voce-Project Lead / Developer

Christopher Cedillo-Tester / QA Engineer
Jagsir Chugha-Research & Deployment Engineer
Swayam Patel-Security Analyst
Mohameddek Hassan-Documentation & Communications

Read [DevREADME.md](DevREADME.md)


