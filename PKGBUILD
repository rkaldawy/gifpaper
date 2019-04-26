# Maintainer: Your Name <youremail@domain.com>
pkgname=gifpaper
pkgver=1.0
pkgrel=1
pkgdesc="Gif wallpapers with X11"
arch=('any')
url="https://gitlab.com/rkaldawy/gifpaper"
license=('GPL3')
groups=()
depends=( 'imlib2>=1.5.1'
          'giblib>=1.2.4'
          'libx11>=1.6.7'
          'libpng>=1.6.37'
          'ffmpeg>=1.4.1' )
makedepends=('git')
install=$pkgname
source=(git+https://gitlab.com/rkaldawy/gifpaper.git)
md5sums=('SKIP') #autofill using updpkgsums

pkgver() {
  cd "$srcdir/$pkgname"
  git describe --long | sed 's/^v//;s/\([^-]*-g\)/r\1/;s/-/./g'
}

build() {
  cd "$srcdir/$pkgname"
  make
}

package() {
  cd "$srcdir/$pkgname"
  make DESTDIR="$pkgdir/" install
}
