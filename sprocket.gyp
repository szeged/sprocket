{
  'variables': {
    'pkg-config': 'pkg-config',
    'chromium_code': 1,
    'grit_out_dir': '<(SHARED_INTERMEDIATE_DIR)/sprocket',
    'sprocket_version': '0.1.0.0',
    'conditions': [
      # On Android disable Linting by default
      ['OS=="android"', {
        'android_lint%': 0,
      }]
    ],
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
        'sprocket_pak',
      ],
      'sources': [
        'app/main_delegate.cc',
        'app/main_delegate.h',
        'browser/browser_context.cc',
        'browser/browser_context.h',
        'browser/browser_main.cc',
        'browser/browser_main.h',
        'browser/browser_main_parts.cc',
        'browser/browser_main_parts.h',
        'browser/content_browser_client.cc',
        'browser/content_browser_client.h',
        'browser/javascript_dialog_manager.cc',
        'browser/javascript_dialog_manager.h',
        'browser/web_contents.cc',
        'browser/web_contents.h',
        'browser/net/url_request_context_getter.h',
        'browser/net/url_request_context_getter.cc',
        'browser/ui/authentication_dialog.cc',
        'browser/ui/authentication_dialog.h',
        'browser/ui/javascript_dialog.cc',
        'browser/ui/javascript_dialog.h',
        'browser/ui/window.cc',
        'browser/ui/window.h',
        'browser/resource_dispatcher_host_delegate.cc',
        'browser/resource_dispatcher_host_delegate.h',
        'common/content_client.cc',
        'common/content_client.h',
        'common/switches.cc',
        'common/switches.h',
      ],
      'cflags': [
        '-g',
      ],
      'conditions': [
        ['OS=="android"', {
          'dependencies': [
            'sprocket_jni_headers',
          ],
          'sources': [
            'android/jni_registrar.cc',
            'android/jni_registrar.h',
            'android/manager.cc',
            'android/manager.h',
            'browser/ui/window_android.cc',
            'browser/ui/authentication_dialog_android.cc',
            'browser/ui/javascript_dialog_android.cc',
            'browser/ui/web_contents_view_delegate_android.cc',
          ],
        }],  # OS=="android"
        ['use_aura==1', {
          'dependencies': [
            '<(DEPTH)/ui/views/controls/webview/webview.gyp:webview',
            '<(DEPTH)/ui/views/views.gyp:views',
            '<(DEPTH)/ui/views/views.gyp:views_test_support',
          ],
          'sources': [
            'browser/ui/context_menu_model.cc',
            'browser/ui/context_menu_model.h',
            'browser/ui/views_delegate_aura.cc',
            'browser/ui/views_delegate_aura.h',
            'browser/ui/window_aura.cc',
            'browser/ui/window_delegate_view_aura.h',
            'browser/ui/window_delegate_view_aura.cc',
            'browser/ui/tab.cc',
            'browser/ui/tab.h',
            'browser/ui/tabbed_pane.cc',
            'browser/ui/tabbed_pane.h',
            'browser/ui/tabbed_pane_listener.h',
            'browser/ui/toolbar.cc',
            'browser/ui/toolbar.h',
          ],
        },{
          'sources': [
            'browser/ui/web_contents_view_delegate.h',
          ],
        }],  # use_aura==1
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
        '<(DEPTH)/ui/views/resources/views_resources.gyp:views_resources',
      ],
      'conditions': [
        ['OS=="android"', {
          'copies': [
            {
              'destination': '<(PRODUCT_DIR)',
              'files': [
                '<(PRODUCT_DIR)/sprocket/assets/sprocket.pak'
              ],
            },
          ],
        }],
      ],
      'actions': [
        {
          'action_name': 'repack_sprocket_pack',
          'variables': {
            'pak_inputs': [
              '<(SHARED_INTERMEDIATE_DIR)/blink/public/resources/blink_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/blink/public/resources/blink_image_resources_100_percent.pak',
              '<(SHARED_INTERMEDIATE_DIR)/content/app/resources/content_resources_100_percent.pak',
              '<(SHARED_INTERMEDIATE_DIR)/content/app/strings/content_strings_en-US.pak',
              '<(SHARED_INTERMEDIATE_DIR)/content/content_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/net/net_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/ui/resources/ui_resources_100_percent.pak',
              '<(SHARED_INTERMEDIATE_DIR)/ui/resources/webui_resources.pak',
              '<(SHARED_INTERMEDIATE_DIR)/ui/strings/app_locale_settings_en-US.pak',
              '<(SHARED_INTERMEDIATE_DIR)/ui/strings/ui_strings_en-US.pak',
              '<(SHARED_INTERMEDIATE_DIR)/ui/views/resources/views_resources_100_percent.pak',
            ],
            'pak_output': '<(PRODUCT_DIR)/sprocket.pak',
            'conditions': [
              ['OS!="android"', {
                'pak_output': '<(PRODUCT_DIR)/sprocket.pak',
              }, {
                'pak_output': '<(PRODUCT_DIR)/sprocket/assets/sprocket.pak',
              }],
            ],
          },
          'includes': [ '../build/repack_action.gypi' ],
        },
      ],
    },
    {
      'target_name': 'chrome_sandbox',
      'type': 'none',
      'dependencies': [
        '<(DEPTH)/sandbox/sandbox.gyp:chrome_sandbox',
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
        'app/main.cc',
      ],
    }],
    'conditions': [
      ['OS=="android"', {
      'targets': [
        {
          # TODO(jrg): Update this action and other jni generators to only
          # require specifying the java directory and generate the rest.
          'target_name': 'sprocket_jni_headers',
          'type': 'none',
          'sources': [
            'android/java/src/hu/uszeged/sprocket/SprocketManager.java',
            'android/java/src/hu/uszeged/sprocket/SprocketWindow.java',
            'android/java/src/hu/uszeged/sprocket/dialogs/SprocketAuthenticationDialog.java',
            'android/java/src/hu/uszeged/sprocket/dialogs/SprocketJavaScriptDialog.java',
          ],
          'variables': {
            'jni_gen_package': 'sprocket',
          },
          'includes': [ '../build/jni_generator.gypi' ],
        },
        {
          'target_name': 'libsprocket_content_view',
          'type': 'shared_library',
          'dependencies': [
            'sprocket_jni_headers',
            'sprocket_lib',
            'sprocket_pak',
            # Skia is necessary to ensure the dependencies needed by
            # WebContents are included.
            '<(DEPTH)/skia/skia.gyp:skia',
            '<(DEPTH)/media/media.gyp:player_android',
          ],
          'sources': [
            'android/library_loader.cc',
          ],
        },
        {
          'target_name': 'sprocket_java',
          'type': 'none',
          'dependencies': [
            '<(DEPTH)/content/content.gyp:content_java',
          ],
          'variables': {
            'java_in_dir': 'android/java',
            'has_java_resources': 1,
            'R_package': 'hu.uszeged.sprocket',
            'R_package_relpath': 'hu/uszeged/sprocket',
          },
          'includes': [ '../build/java.gypi' ],
        },
        {
          # sprocket_apk creates a .jar as a side effect. Any java targets
          # that need that .jar in their classpath should depend on this target,
          # sprocket_apk_java. Dependents of sprocket_apk receive its
          # jar path in the variable 'apk_output_jar_path'. This target should
          # only be used by targets which instrument sprocket_apk.
          'target_name': 'sprocket_apk_java',
          'type': 'none',
          'dependencies': [
            'sprocket_apk',
          ],
          'includes': [ '../build/apk_fake_jar.gypi' ],
        },
        {
          'target_name': 'sprocket_manifest',
          'type': 'none',
          'variables': {
            'jinja_inputs': ['android/sprocket_apk/AndroidManifest.xml.jinja2'],
            'jinja_output': '<(SHARED_INTERMEDIATE_DIR)/sprocket_manifest/AndroidManifest.xml',
          },
          'includes': [ '../build/android/jinja_template.gypi' ],
        },
        {
          'target_name': 'sprocket_apk',
          'type': 'none',
          'dependencies': [
            'sprocket_icudata',
            'sprocket_v8_external_data',
            'sprocket_java',
            'libsprocket_content_view',
            '<(DEPTH)/content/content.gyp:content_java',
            '<(DEPTH)/base/base.gyp:base_java',
            '<(DEPTH)/media/media.gyp:media_java',
            '<(DEPTH)/net/net.gyp:net_java',
            '<(DEPTH)/third_party/mesa/mesa.gyp:osmesa_in_lib_dir',
            '<(DEPTH)/tools/android/forwarder/forwarder.gyp:forwarder',
            '<(DEPTH)/tools/imagediff/image_diff.gyp:image_diff#host',
            '<(DEPTH)/ui/android/ui_android.gyp:ui_java',
          ],
          'variables': {
            'apk_name': 'Sprocket',
            'manifest_package_name': 'hu.uszeged.sprocket_apk',
            'android_manifest_path': '<(SHARED_INTERMEDIATE_DIR)/sprocket_manifest/AndroidManifest.xml',
            'java_in_dir': 'android/sprocket_apk',
            'resource_dir': 'android/sprocket_apk/res',
            'native_lib_target': 'libsprocket_content_view',
            'additional_input_paths': ['<(PRODUCT_DIR)/sprocket/assets/sprocket.pak'],
            'asset_location': '<(PRODUCT_DIR)/sprocket/assets',
            'extra_native_libs': ['<(SHARED_LIB_DIR)/libosmesa.so'],
            'conditions': [
              ['icu_use_data_file_flag==1', {
                'additional_input_paths': [
                  '<(PRODUCT_DIR)/icudtl.dat',
                ],
              }],
              ['v8_use_external_startup_data==1', {
                'additional_input_paths': [
                  '<(PRODUCT_DIR)/natives_blob.bin',
                  '<(PRODUCT_DIR)/snapshot_blob.bin',
                ],
              }],
            ],
          },
          'includes': [ '../build/java_apk.gypi' ],
        },
        {
          'target_name': 'sprocket_icudata',
          'type': 'none',
          'conditions': [
            ['icu_use_data_file_flag==1', {
              'copies': [
                {
                  'destination': '<(PRODUCT_DIR)/sprocket/assets',
                  'files': [
                    '<(PRODUCT_DIR)/icudtl.dat',
                  ],
                },
              ],
            }],
          ],
        },
        {
          'target_name': 'sprocket_v8_external_data',
          'type': 'none',
          'conditions': [
            ['v8_use_external_startup_data==1', {
              'copies': [
                {
                  'destination': '<(PRODUCT_DIR)/sprocket/assets',
                  'files': [
                    '<(PRODUCT_DIR)/natives_blob.bin',
                    '<(PRODUCT_DIR)/snapshot_blob.bin',
                  ],
                },
              ],
            }],
          ],
        },
      ],
    }]  # OS=="android"
    ]
}
