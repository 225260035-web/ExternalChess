# Modern Turkish Chess Engine

Modern ve güçlü bir satranç motoru. Stockfish değerlendirme fonksiyonları ile gelişmiş arama algoritmaları kullanır.

## Özellikler

### Motor Özellikleri
- **Gelişmiş Değerlendirme**: Stockfish tabanlı değerlendirme fonksiyonları
- **Modern Arama**: Negamax, Alpha-Beta budama, iterative deepening
- **Pozisyonel Anlayış**: Gelişmiş pawn yapısı, mobilite ve kral güvenliği değerlendirmesi
- **Endgame Bilgisi**: Özel endgame değerlendirme fonksiyonları
- **Hash Tabloları**: Material ve pawn hash tabloları ile performans optimizasyonu

### Arayüz Özellikleri  
- **Modern GUI**: SFML 2.5.1 ile oluşturulmuş kullanıcı dostu arayüz
- **UCI Desteği**: Universal Chess Interface protokolü
- **Değerlendirme Göstergesi**: Pozisyon değerlendirmesi ve arama derinliği gösterimi
- **Hamle Geçmişi**: Oyun geçmişi ve notasyon desteği
- **Esnek Ayarlar**: Arama derinliği ve motor ayarları

## Kurulum

### Sistem Gereksinimleri
- C++17 destekli derleyici (GCC 9+, Clang 9+, MSVC 2019+)
- CMake 3.16+
- SFML 2.5.1 (GUI için)

### Derleme

#### Linux/macOS
```bash
# Bağımlılıkları yükleyin
sudo apt install libsfml-dev cmake build-essential  # Ubuntu/Debian
# veya
brew install sfml cmake  # macOS

# Projeyi derleyin
mkdir build && cd build
cmake ..
make -j$(nproc)
```

#### Windows
```bash
# SFML'i yükleyin ve CMake ile yapılandırın
mkdir build && cd build
cmake .. -DSFML_ROOT="C:/SFML-2.5.1"
cmake --build . --config Release
```

### Çalıştırma

#### GUI Versiyonu
```bash
./turkish_chess_gui
```

#### Konsol/UCI Versiyonu  
```bash
./turkish_chess_engine
```

## Kullanım

### GUI Kontrolları
- **Sol Klik**: Taş seçimi ve hamle yapma
- **Sağ Klik**: Seçimi iptal etme
- **F**: Tahtayı çevirme
- **U**: Son hamleyi geri alma
- **N**: Yeni oyun
- **Enter**: Motor hamlesi

### Konsol Komutları
```
position startpos          - Başlangıç pozisyonu
position fen <fen>         - FEN'den pozisyon
go                         - Arama başlat
eval                       - Pozisyon değerlendirmesi  
perft <depth>              - Perft testi
board                      - Tahtayı göster
uci                        - UCI moduna geç
quit                       - Çıkış
```

## Motor Ayarları

### UCI Seçenekleri
- **Threads**: Arama iş parçacığı sayısı (1-128)
- **Hash**: Hash tablosu boyutu MB (1-2048)

### Arama Parametreleri
- **Derinlik**: Maksimum arama derinliği (1-20)
- **Zaman Yönetimi**: Otomatik zaman kontrolü

## Performans

### Arama Hızı
- ~1-5 Million nodes/second (CPU'ya bağlı)
- Etkili budama teknikleri ile yüksek verimlilik

### Değerlendirme Gücü
- Detaylı pozisyonel değerlendirme
- Material imbalance hesaplaması
- Gelişmiş pawn yapısı analizi
- Kral güvenliği değerlendirmesi

## Teknik Detaylar

### Arama Algoritması
- **Negamax**: Ana arama algoritması
- **Alpha-Beta Pruning**: Etkili budama
- **Iterative Deepening**: Progressif derinlik artışı
- **Move Ordering**: Etkili hamle sıralaması
- **Transposition Table**: Hash tablosu optimizasyonu

### Değerlendirme Fonksiyonları
- **Material**: Taş değerleri ve imbalance
- **Mobility**: Taş mobilitesi
- **Pawn Structure**: Pawn zincirleri ve zayıflıklar
- **King Safety**: Kral güvenliği
- **Threats**: Saldırı ve tehditler
- **Space**: Alan kontrolü
- **Endgame**: Özel endgame değerlendirmeleri

## Geliştirme

### Kod Yapısı
```
src/
├── types.h              # Temel türler ve sabitler
├── bitboard.h/cpp       # Bitboard operasyonları
├── position.h/cpp       # Pozisyon temsili
├── movegen.h/cpp        # Hamle üretimi
├── search.h/cpp         # Arama algoritmaları  
├── evaluate.h/cpp       # Değerlendirme
├── ui/                  # SFML arayüzü
└── main.cpp            # Ana program

external_eval/          # Stockfish değerlendirme
├── evaluate.cpp
├── material.cpp
├── pawns.cpp
├── endgame.cpp
└── psqt.cpp
```

### Katkıda Bulunma
1. Repository'yi fork edin
2. Feature branch oluşturun (`git checkout -b feature/amazing-feature`)
3. Değişikliklerinizi commit edin (`git commit -m 'Add amazing feature'`)
4. Branch'i push edin (`git push origin feature/amazing-feature`)
5. Pull Request oluşturun

## Lisans

Bu proje GPL v3 lisansı altında dağıtılmaktadır. Detaylar için `LICENSE` dosyasına bakın.

## Teşekkürler

- **Stockfish Team**: Değerlendirme fonksiyonları ve algoritmalar için
- **SFML Team**: Mükemmel multimedia library için
- **Chess Programming Community**: Bilgi paylaşımı için

## İletişim

- **Geliştirici**: Turkish Chess Engine Team
- **E-posta**: info@turkishchessengine.com
- **GitHub**: https://github.com/turkishchessengine/modern-engine

---

**Modern Turkish Chess Engine** - Güçlü, hızlı ve kullanıcı dostu satranç motoru.