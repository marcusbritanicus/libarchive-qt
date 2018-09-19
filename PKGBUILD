#Maintainer: Marcus Britanicus

pkgname=libarchive-qt-git
pkgver=r18.16d35b7
pkgrel=1
pkgdesc="A Qt based archiving solution with libarchive backend"
arch=('x86_64')
url="https://gitlab.com/marcusbritanicus/libarchive-qt"
license=('LGPL3')
depends=( 'libarchive' 'qt5-base' 'zlib' 'bzip2' 'xz')
makedepends=('git' )
provides=('libarchive-qt')
conflicts=('libarchive-qt')
source=("${pkgname}::git+https://gitlab.com/marcusbritanicus/libarchive-qt.git")
md5sums=('SKIP')

pkgver() {
  cd ${pkgname}
  printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

prepare() {
  mkdir -p build
}

build() {
  cd build
  qmake-qt5 ../$pkgname PREFIX=/usr
  make
}

package() {
  cd build
  make INSTALL_ROOT="$pkgdir" install
}
