import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:window_to_second_display/window_to_second_display.dart';

void main() {
  const MethodChannel channel = MethodChannel('window_to_second_display');

  TestWidgetsFlutterBinding.ensureInitialized();

  setUp(() {
    channel.setMockMethodCallHandler((MethodCall methodCall) async {
      return 'window moved';
    });
  });

  tearDown(() {
    channel.setMockMethodCallHandler(null);
  });

  test('getPlatformVersion', () async {
    expect(
        (await WindowToSecondDisplay.moveWindow).toString().contains('moved'),
        true);
  });
}
