#Maintainer: Marcus Britanicus

pkgname=libarchive-qt
pkgver=v1.0.2
pkgrel=1
pkgdesc="A Qt based archiving solution with libarchive backend."
arch=('x86_64')
url="https://gitlab.com/marcusbritanicus/libarchive-qt"
license=('LGPL-3.0')
depends=('libarchive' 'qt5-base' 'zlib' 'bzip2' 'xz')
makedepends=('git')
provides=('libarchive-qt')
conflicts=('libarchive-qt')
source=("https://gitlab.com/marcusbritanicus/libarchive-qt/-/archive/$pkgver/libarchive-qt-$pkgver.tar.gz")
md5sums=('SKIP')

prepare() {
  mkdir -p build
}

build() {
  cd ${pkgname}-${pkgver}

  qmake-qt5
  make
}

package() {
  cd ${pkgname}-${pkgver}
  make INSTALL_ROOT=${pkgdir} install
}