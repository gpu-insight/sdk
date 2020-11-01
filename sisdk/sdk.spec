Name:       botson-sdk
Version:    %{sdk_version}
Release:	%{sdk_release}%{?dist}
Summary:    Botson GL SDK

Group:	Botson		
License:	GPL
URL:		http://gitlab.botson.com/bts_repos/gb/udriver/btx_sdk
Source0: %{name}-%{version}.tar.gz

%description
The OpenGL SDK distributed by botson

%prep
%setup -q %{name}-%{version}


%build
%ifarch x86_64
(echo "x86_64")
meson build
ninja-build -C build
%endif

%install
mkdir -p %{buildroot}/usr/local

%ifarch x86_64
(echo "install x86_64")
meson configure -Dprefix=%{buildroot}/usr/local build
ninja-build -C build install
%endif

%files
/usr/local/*

%doc

%changelog

