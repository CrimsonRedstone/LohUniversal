Embedded fonts (OFL)
====================

Filenames have no hyphens so JUCE BinaryData identifiers are stable
(InterRegular_ttf, not Inter_Regular_ttf).

Inter Regular / Bold / Display Bold
  InterRegular.ttf  InterBold.ttf  InterDisplayBold.ttf
  https://github.com/rsms/inter
  SIL Open Font License 1.1
  Cyrillic + Latin. Used for UI and the "Лох" wordmark.

Great Vibes Regular
  GreatVibesRegular.ttf
  https://github.com/google/fonts  (ofl/greatvibes)
  SIL Open Font License 1.1
  Script face for "Universal".

These are compiled into the plugin via juce_add_binary_data so a
vanilla Windows box still draws Cyrillic, ·, –, ±, ¢.
