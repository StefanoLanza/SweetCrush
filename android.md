# Android Build Troubleshooting

## Signing
* Generate a key with
```
keytool.exe -genkey -v -keystore sweetcrush.jks -keyalg RSA -keysize 2048 -validity 10000 -alias sweetcrush
```
This will prompt a password and other info
* in keystore.properties, set the same alias and password

## Missing assets
* Assets should be placed under bin/assets
* Execute `gradlew checkAssetPaths ` to verify they are correctly found and packaged