Name: battery-monitor
Version: 0.1
Release: alt1

Summary: Displays battery percentage notifications if it is less than a 30%
License: GPLv3+
Group: other

BuildRequires: make
BuildRequires: gcc-c++
BuildRequires: libsystemd-devel

Source0: %name-%version.tar

%description
This program displays notification every 20 min if users battery percentage is less than 30%.

%prep
%setup -q

%build
%make_build

%install
mkdir -p \
    %buildroot/bin
install -Dm0755 %name %buildroot/bin/
mkdir -p \
    %buildroot%_sysconfdir/xdg/systemd/user/
cp %name.timer %name.service \
    %buildroot%_sysconfdir/xdg/systemd/user/

%files
/bin/%name
/etc/xdg/systemd/user/%name.service
/etc/xdg/systemd/user/%name.timer