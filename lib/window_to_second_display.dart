import 'dart:async';

import 'package:flutter/services.dart';

class WindowToSecondDisplay {
  static const MethodChannel _channel =
      const MethodChannel('window_to_second_display');

  static Future<String> get moveWindow async {
    final String version = await _channel.invokeMethod('moveWindow');
    return version;
  }
}
