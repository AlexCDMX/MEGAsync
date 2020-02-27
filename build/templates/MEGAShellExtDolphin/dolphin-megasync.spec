Name:       dolphin-megasync
Version:    EXT_VERSION
Release:	%(cat MEGA_BUILD_ID || echo "1").1
Summary:	Extension for Dolphin to interact with Megasync
License:	Freeware
Group:		Applications/Others
Url:		https://mega.nz
Source0:	dolphin-megasync_%{version}.tar.gz
Vendor:		MEGA Limited
Packager:	MEGA Linux Team <linux@mega.co.nz>

AutoReq: 0

#BuildRequires:  libqt4-dev, kdelibs5-dev, cmake
BuildRequires:  qt-devel
%if 0%{?suse_version}
BuildRequires:  libkde4-devel
%endif
%if 0%{?sle_version} >= 120100 || 0%{?suse_version} > 1320
BuildRequires:  kdelibs4support extra-cmake-modules libQt5Core-devel libQt5Network-devel kio-devel
%endif
%if 0%{?fedora}
BuildRequires:  kdelibs, kdelibs-devel
%if 0%{?fedora_version} <= 23
BuildRequires: qca2
%endif
%if 0%{?fedora_version} >= 22
# Fedora 21 cmake is too old for KF5
BuildRequires: kf5-kdelibs4support-devel extra-cmake-modules
%endif
%endif

%if 0%{?rhel_version} || 0%{?scientificlinux_version}
BuildRequires: qt-devel kdelibs-devel gcc-c++
%endif


%if 0%{?centos_version}
BuildRequires: qt-devel kdelibs-devel
%endif

Requires:       megasync >= 3.5

%description
Secure:
Your data is encrypted end to end. Nobody can intercept it while in storage or in transit.

Flexible:
Sync any folder from your PC to any folder in the cloud. Sync any number of folders in parallel.

Fast:
Take advantage of MEGA's high-powered infrastructure and multi-connection transfers.

Generous:
Store up to 50 GB for free!

%prep
%setup -q

%build

# Create a temporary file containing the list of files
EXTRA_FILES=%{buildroot}/ExtraFiles.list
touch %{EXTRA_FILES}

cmake -DCMAKE_INSTALL_PREFIX="`kde4-config --prefix`" $PWD
make
make install DESTDIR=%{buildroot}

echo %(kde4-config --path services | awk -NF ":" '{print $NF}')/megasync-plugin.desktop  >> %{EXTRA_FILES}
echo %(kde4-config --path module | awk -NF ":" '{print $NF}')/megasyncplugin.so >> %{EXTRA_FILES}

if which kf5-config >/dev/null; then
%if 0%{?fedora_version} >= 26 || 0%{?suse_version} > 1320
rm megasync-plugin.moc
%endif
rm -r CMakeFiles
rm CMakeLists.txt
mv CMakeLists_kde5.txt CMakeLists.txt
cmake -DCMAKE_INSTALL_PREFIX="`kf5-config --prefix`" $PWD
make
make install DESTDIR=%{buildroot}
#fix issue with compilation of megasync-plugin-overlay.cpp lacking of symbols: replace with a precompiled library
%if 0%{?fedora_version} >= 26
mv megasyncdolphinoverlayplugin.so_fed%{?fedora_version} %{buildroot}/%(kf5-config --path lib | awk -NF ":" '{print $1}')/qt5/plugins/kf5/overlayicon/megasyncdolphinoverlayplugin.so || mv megasyncdolphinoverlayplugin.so_fed27 %{buildroot}/%(kf5-config --path lib | awk -NF ":" '{print $1}')/qt5/plugins/kf5/overlayicon/megasyncdolphinoverlayplugin.so

%endif
%if 0%{?suse_version} > 1320
mv megasyncdolphinoverlayplugin.so_ostum %{buildroot}/%(kf5-config --path lib | awk -NF ":" '{print $1}')/qt5/plugins/kf5/overlayicon/megasyncdolphinoverlayplugin.so
%endif

echo %(kf5-config --path services | awk -NF ":" '{print $NF}')/megasync-plugin.desktop >> %{EXTRA_FILES}
echo %(kf5-config --path lib | awk -NF ":" '{print $1}')/qt5/plugins/megasyncplugin.so >> %{EXTRA_FILES}

if [ -d %{buildroot}/%(kf5-config --path lib | awk -NF ":" '{print $1}')/qt5/plugins/kf5/overlayicon ]; then
echo %(kf5-config --path lib | awk -NF ":" '{print $1}')/qt5/plugins/kf5/overlayicon/megasyncdolphinoverlayplugin.so >> %{EXTRA_FILES}
echo %(kf5-config --path lib | awk -NF ":" '{print $1}')/qt5/plugins/kf5/overlayicon >> %{EXTRA_FILES}
echo %(kf5-config --path lib | awk -NF ":" '{print $1}')/qt5/plugins/kf5 >> %{EXTRA_FILES}
echo '%{_datadir}/icons/hicolor/*/*/mega-*.png' >> %{EXTRA_FILES}
echo '%{_datadir}/icons/hicolor/*/*' >> %{EXTRA_FILES}
fi

fi

%if 0%{?centos_version} || 0%{?rhel_version} || 0%{?scientificlinux_version}
#fix conflict with existing /usr/lib64 (pointing to /usr/lib)
if [ -d %{buildroot}/usr/lib ]; then
    rsync -av %{buildroot}/usr/lib/ %{buildroot}/usr/lib64/
    rm -rf %{buildroot}/usr/lib
fi
%endif

%clean
echo cleaning
%{?buildroot:%__rm -rf "%{buildroot}"}

%files -f %{EXTRA_FILES}
%defattr(-,root,root)



%changelog
