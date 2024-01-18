## NBGL serialization/deserialization library

### Usage

Deserialize raw bytes into an Nbgl event:
```python
>>> from nbgl_lib import deserialize_nbgl_bytes
>>> data = bytes.fromhex("00010501f403e800ff003201000301020403015465737420627574746f6e00")
>>> nbgl_event = deserialize_nbgl_bytes(data)
>>> nbgl_event
NbglDrawObjectEvent(obj=NbglButton(area=NbglArea(width=255, height=50, x0=500, y0=1000, background_color=<NbglColor.DARK_GRAY: 1>, bpp=<NbglBpp.BPP_1: 0>), inner_color=<NbglColor.WHITE: 3>, border_color=<NbglColor.DARK_GRAY: 1>, foreground_color=<NbglColor.LIGHT_GRAY: 2>, radius=<NbglRadius.RADIUS_40_PIXELS: 4>, font_id=<NbglFontId.BAGL_FONT_HM_ALPHA_MONO_MEDIUM_32px: 3>, localized=True, text='Test button'))
```

Serialize a Nbgl event into a json-like dict:
```python
>>> from nbgl_lib import serialize_nbgl_json
>>> serialize_nbgl_json(nbgl_event)
{'event': 'NBGL_DRAW_OBJ', 'obj': {'type': 'BUTTON', 'content': {'area': {'width': 255, 'height': 50, 'x0': 500, 'y0': 1000, 'background_color': 'DARK_GRAY', 'bpp': 'BPP_1'}, 'inner_color': 'WHITE', 'border_color': 'DARK_GRAY', 'foreground_color': 'LIGHT_GRAY', 'radius': 'RADIUS_40_PIXELS', 'font_id': 'BAGL_FONT_HM_ALPHA_MONO_MEDIUM_32px', 'localized': True, 'text': 'Test button'}}}
```

Deserialize json-like dict into an Nbgl event:

```python
>>> from nbgl_lib import deserialize_nbgl_json
>>> deserialize_nbgl_json({'event': 'NBGL_DRAW_OBJ', 'obj': {'type': 'BUTTON', 'content': {'area': {'width': 255, 'height': 50, 'x0': 500, 'y0': 1000, 'background_color': 'DARK_GRAY', 'bpp': 'BPP_1'}, 'inner_color': 'WHITE', 'border_color': 'DARK_GRAY', 'foreground_color': 'LIGHT_GRAY', 'radius': 'RADIUS_40_PIXELS', 'font_id': 'BAGL_FONT_HM_ALPHA_MONO_MEDIUM_32px', 'localized': True, 'text': 'Test button'}}})
NbglDrawObjectEvent(obj=NbglButton(area=NbglArea(width=255, height=50, x0=500, y0=1000, background_color=<NbglColor.DARK_GRAY: 1>, bpp=<NbglBpp.BPP_1: 0>), inner_color=<NbglColor.WHITE: 3>, border_color=<NbglColor.DARK_GRAY: 1>, foreground_color=<NbglColor.LIGHT_GRAY: 2>, radius=<NbglRadius.RADIUS_40_PIXELS: 4>, font_id=<NbglFontId.BAGL_FONT_HM_ALPHA_MONO_MEDIUM_32px: 3>, localized=True, text='Test button'))
```
### Tests

- ```generate_data_test.c``` generates reference data for deserialization testing.
- ```test_bytes_deserialize.py``` deserializes the reference data and inspect the result.
- ```test_json_ser_deser.py``` tests json serialization and deserialization.

To generate reference data and run tests:

```
make run_test
```
