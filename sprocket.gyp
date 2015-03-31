{
  'variables': {
    'pkg-config': 'pkg-config',
    'chromium_code': 1,
    'grit_out_dir': '<(SHARED_INTERMEDIATE_DIR)/sprocket',
    'sprocket_version': '0.1.0.0',
  },
  'targets': [
    {
      'target_name': 'sprocket_lib',
      'type': 'static_library',
      'defines': ['SPROCKET_VERSION="<(sprocket_version)"'],
      'variables': {
        'chromium_code': 1,
      },
      'include_dirs': [
        '.',
        '<(DEPTH)/third_party/WebKit/public/platform',
        '<(DEPTH)/third_party/WebKit/public/web',
        '<(grit_out_dir)',
        '<(SHARED_INTERMEDIATE_DIR)/ui/resources',
        '<(SHARED_INTERMEDIATE_DIR)/ui/strings',
        '<(SHARED_INTERMEDIATE_DIR)/webkit',
      ],
      'dependencies': [
        # Content dependencies
        '<(DEPTH)/content/content.gyp:content_app_both',
        '<(DEPTH)/content/content.gyp:content_common',
        '<(DEPTH)/content/content.gyp:content_gpu',
        '<(DEPTH)/content/content.gyp:content_plugin',
        '<(DEPTH)/content/content.gyp:content_ppapi_plugin',
        '<(DEPTH)/content/content.gyp:content_renderer',
        '<(DEPTH)/content/content.gyp:content_resources',
        '<(DEPTH)/content/content.gyp:content_utility',
        '<(DEPTH)/content/content.gyp:content_browser',
        # Fix mojo include error.
        '<(DEPTH)/content/content_common_mojo_bindings.gyp:content_common_mojo_bindings_mojom',
        # UI dependencies
        '<(DEPTH)/ui/views/controls/webview/webview.gyp:webview',
        '<(DEPTH)/ui/views/views.gyp:views',
        '<(DEPTH)/ui/views/views.gyp:views_test_support',
        'sprocket_pak',
      ],
      'sources': [
        'browser/browser_context.cc',
        'browser/browser_context.h',
        'browser/browser_main.cc',
        'browser/browser_main.h',
        'browser/browser_main_parts.cc',
        'browser/browser_main_parts.h',
        'browser/content_browser_client.cc',
        'browser/content_browser_client.h',
        'browser/net/network_delegate.h',
        'browser/net/network_delegate.cc',
        'browser/net/url_request_context_getter.h',
        'browser/net/url_request_context_getter.cc',
        'browser/ui/context_menu_model.cc',
        'browser/ui/context_menu_model.h',
        'browser/ui/web_contents.cc',
        'browser/ui/web_contents.h',
        'browser/ui/web_contents_views.cc',
        'common/content_client.cc',
        'common/content_client.h',
      ],
      'cflags': [
        '-g',
      ],
    },
    {
      'target_name': 'sprocket_resources',
      'type': 'none',
      'actions': [
        {
          'action_name': 'generate_sprocket_resources',
          'variables': {
            'grit_grd_file': 'sprocket_resources.grd',
          },
          'includes': [ '../build/grit_action.gypi' ],
        },
      ],
      'includes': [ '../build/grit_target.gypi' ],
      'copies': [
        {
          'destination': '<(PRODUCT_DIR)',
          'files': [
            '<(grit_out_dir)/sprocket_resources.pak'
          ],
        },
      ],
    },
    {
      'target_name': 'sprocket_pak',
      'type': 'none',
      'dependencies': [
        '<(DEPTH)/content/app/resources/content_resources.gyp:content_resources',
        '<(DEPTH)/content/app/strings/content_strings.gyp:content_strings',
        '<(DEPTH)/content/content.gyp:content_resources',
        'sprocket_resources',
        '<(DEPTH)/net/net.gyp:net_resources',
        '<(DEPTH)/ui/resources/ui_resources.gyp:ui_resources',
        '<(DEPTH)/ui/strings/ui_strings.gyp:ui_strings',
        'sprocket_resources',
      ],
      'actions': [
        {
          'action_name': 'repack_sprocket_pack',
          'variables': {
            'pak_inputs': [
              '<(SHARED_INTERMEDIATE_DIR)/blink/public/resources/blink_resources_100_percent.pak',
              '<(SHARED_INTERMEDIATE_DIR)/content/app/resources/content_resources_100_percent.pak',
              '<(SHARED_INTERMEDIATE_DIR)/content/app/strings/content_strings_en-US.pak',
              '<(SHARED_INTERMEDIATE_DIR)/content/browser/tracing/tracing_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/content/content_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/content/shell_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/net/net_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/ui/resources/ui_resources_100_percent.pak',
              '<(SHARED_INTERMEDIATE_DIR)/ui/resources/webui_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/ui/strings/app_locale_settings_en-US.pak',
              '<(SHARED_INTERMEDIATE_DIR)/ui/strings/ui_strings_en-US.pak',
            ],
            'pak_output': '<(PRODUCT_DIR)/sprocket.pak',
          },
          'includes': [ '../build/repack_action.gypi' ],
        },
      ],
    },
    {
      'target_name': 'sprocket',
      'type': 'executable',
      'dependencies': [
        'sprocket_lib',
      ],
      'include_dirs': [
        '.',
      ],
      'sources': [
        'main.cc',
        'main_delegate.cc',
        'main_delegate.h',
      ],
    }]
}
