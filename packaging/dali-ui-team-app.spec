Name:       dali-ui-team-app
Summary:    A three-window DALi Team application template
Version:    1.0.0
Release:    1
Group:      Applications/Graphics
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
BuildRequires: cmake >= 3.20
BuildRequires: gettext-tools
BuildRequires: python3
BuildRequires: pkgconfig
BuildRequires: pkgconfig(libtzplatform-config)
BuildRequires: pkgconfig(team-application) >= 1.3.6
BuildRequires: pkgconfig(app-core-cpp)
BuildRequires: pkgconfig(capi-base-common)
BuildRequires: pkgconfig(dali2-core)
BuildRequires: pkgconfig(dali2-adaptor)
BuildRequires: pkgconfig(dali2-ui-foundation)
Requires: team-application >= 1.3.6

%description
Three Team UI members with translucent windows, translated labels and
keyboard focus navigation. The main member selects the application language.

%define app_ro_dir %TZ_SYS_RO_APP/%{name}
%prep
%setup -q
%build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=%{app_ro_dir}
cmake --build build -- %{?jobs:-j%jobs}
%install
DESTDIR=%{buildroot} cmake --install build
mkdir -p %{buildroot}%TZ_SYS_RO_PACKAGES
install -m 0644 packaging/%{name}.xml %{buildroot}%TZ_SYS_RO_PACKAGES/%{name}.xml
%check
ctest --test-dir build --output-on-failure
python3 tests/check-installed.py %{buildroot} %{app_ro_dir} %TZ_SYS_RO_PACKAGES
%files
%manifest packaging/%{name}.manifest
%defattr(-,root,root,-)
%license LICENSE
%{app_ro_dir}/bin/*.so
%{app_ro_dir}/res/locale/*/LC_MESSAGES/*.mo
%TZ_SYS_RO_PACKAGES/%{name}.xml
