IoT Manager

Autoři:
Dmitrii Ivanushkin (xivanu00) - kapitán
Oleh Krasovskyi (xkrasoo00)

ADRESÁŘOVÁ STRUKTURA

ITU Projekt
|-- frontend/           # Zdrojové kódy React Native aplikace
|   |-- app/            # Routování a obrazovky (Expo Router)
|   |-- components/     # Znovupoužitelné UI komponenty
|   |-- lib/            # Pomocné knihovny, API klient, schémata
|-- backend/            # Zdrojové kódy FastAPI backendu

Poznámka: Soubory v kořenu (app.json, eas.json) a složka /assets byly
vygenerovány frameworkem Expo.

Dmitrii Ivanushkin (xivanu00):
1. Nastavení projektu, metasoubory s definicí rozcestníků a přesměrování:
   - /app/_layout.tsx
   - /app/index.tsx
   - /app/(tabs)/_layout.tsx
   - /app/(tabs)/widgets/_layout.tsx
   - /app/(tabs)/devices/_layout.tsx
2. Stránka se seznamem widgetů, skupiny widgetů a pomocné komponenty:
   - /app/(tabs)/widgets/index.tsx
   - /components/Toast.tsx
   - /components/WidgetCard.tsx
   - /components/WidgetChart.tsx
   - /components/WidgetGroupTabs.tsx
3. Stránka detailu widgetu a pomocné komponenty:
   - /app/(tabs)/widgets/[id].tsx
   - /components/AppearanceSheet.tsx
   - /components/Collapsible.tsx
   - /components/PollingSlider.tsx
   - /components/SelectionSheet.tsx
4. Pomocné funkce pro práci s API:
   - /lib/api/config.ts
   - /lib/api/types.ts
   - /lib/api/widgets/*.ts
   - /lib/api/widgetGroups/*.ts
5. Konstanty pro práci s ikonami a barvami:
   - /lib/constants/*.ts
6. Validační schémata pro PUT a PATCH akce:
   - /lib/schemas/widgetSchemas.ts
   - /lib/schemas/widgetGroupSchemas.ts

Oleh krasovskyi (xkrasoo00):
1. Stránka se skupinami zařízení a zařízeními přiřazenými k těmto skupinám, pomocné komponenty:
   - /app/(tabs)/devices/index.tsx
   - /components/DeviceListItem.tsx
   - /components/GroupListItem.tsx
2. Stránka pro správu zařízení, topiců a příkazů:
   - /app/(tabs)/devices/[id].tsx
3. Pomocné funkce pro práci s API:
   - /lib/api/commands/*.ts
   - /lib/api/devices/*.ts
   - /lib/api/topics/*.ts
   - /lib/api/groups/*.ts
   - /lib/api/types.ts
4. Validační schémata pro operace PUT a PATCH nad příkazy (commands), zařízeními (devices) a skupinami zařízení (device groups):
   - /lib/schemas/commandSchema.ts
   - /lib/schemas/device.ts
   - /lib/schemas/deviceGroup.ts
   - /lib/schemas/deviceSettingsSchema.ts
5. Pomocné utility:
   - /lib/utils/generateRandomToken.ts


PRÁCE S APLIKACÍ

1. Instalace
   1.1. Otevřete terminál.
   1.2. Přejděte do složky frontendu:
        cd frontend
   1.3. Nainstalujte potřebné závislosti:
        npm install
   1.4. Vytvořte lokální konfigurační soubor:
        cp .env.example .env.local

2. Nastavení prostředí
   2.1. Otevřete soubor .env.local a přidejte URL backendu:
        EXPO_PUBLIC_API_URL=http://localhost:8000

3. Spuštění
   3.1. Ujistěte se, že váš počítač a mobilní telefon jsou připojeny ke stejné Wi-Fi síti.
   3.2. Spusťte vývojový server:
        npm run start

4. Použití na mobilním zařízení
   4.1. Nainstalujte si aplikaci "Expo Go" (dostupná na Google Play pro Android a App Store pro iOS).
   4.2. Spusťte Expo Go a naskenujte QR kód, který se zobrazil v terminálu.
