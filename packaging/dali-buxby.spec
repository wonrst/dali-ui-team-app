Name:       dali-buxby
Summary:    Samsong Buxby - a three-window DALi Team reference application
Version:    1.0.0
Release:    1
Group:      Applications/Graphics
License:    Apache-2.0
URL:        https://github.com/dalihub/dali-ui
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
BuildRequires: pkgconfig(dali2-ui-components)
Requires: team-application >= 1.3.6

%description
Three Team UI members: a conversation window, an afternoon schedule and a
weekly activity chart. Each member translates its own gettext domain in the
Team language callback. Main alone sets the application language through the
app-core package's installed API header. No DALi devel or integration API is
used by the target app. The separate host preview is not packaged.

%define app_ro_dir %TZ_SYS_RO_APP/%{name}
%prep
%setup -q
%build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=%{app_ro_dir} -DBUXBY_HOST_PREVIEW=OFF
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
