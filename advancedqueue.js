(function ($) {
  $(function() {
    return;

    var element_settings = {
      'event': 'refreshRequest',
      'progress': { 'type': 'throbber' },
      'url': 'queues'
    };

    // Bind AJAX behaviors to all items showing the class.
    $('.views-ajax-link').once('views-ajax-processed').each(function () {
      var element_settings = base_element_settings;
      // Set the URL to go to the anchor.
      if ($(this).attr('href')) {
        element_settings.url = $(this).attr('href');
      }
      var base = $(this).attr('id');
      Drupal.ajax[base] = new Drupal.ajax(base, this, element_settings);
    });

  });

  /**
   * Handler for the form serialization.
   *
   * Runs before the beforeSend() handler (see below), and unlike that one, runs
   * before field data is collected.
   */
  var simpleBeforeSerialize = function (element, options) {
    // Allow detaching behaviors to update field values before collecting them.
    // This is only needed when field values are added to the POST data, so only
    // when there is a form such that this.form.ajaxSubmit() is used instead of
    // $.ajax(). When there is no form and $.ajax() is used, beforeSerialize()
    // isn't called, but don't rely on that: explicitly check this.form.
    if (this.form) {
      var settings = this.settings || Drupal.settings;
      Drupal.detachBehaviors(this.form, settings, 'serialize');
    }
  };

  Drupal.behaviors.ViewsAjaxView.attach = function() {
    if (Drupal.settings && Drupal.settings.views && Drupal.settings.views.ajaxViews) {
      // Retrieve the path to use for views' ajax.
      var ajax_path = Drupal.settings.views.ajax_path;

      // If there are multiple views this might've ended up showing up multiple times.
      if (ajax_path.constructor.toString().indexOf("Array") != -1) {
        ajax_path = ajax_path[0];
      }

      $.each(Drupal.settings.views.ajaxViews, function(i, settings) {
        var view = '.view-dom-id-' + settings.view_dom_id;
        var element_settings = {
          url: ajax_path,
          submit: settings,
          setClick: true,
          event: 'refreshRequested',
          selector: view,
          progress: { type: 'none' }
        };

        $(view).filter(':not(.views-refresh-processed)')
          // Don't attach to nested views. Doing so would attach multiple behaviors
          // to a given element.
          .filter(function() {
            // If there is at least one parent with a view class, this view
            // is nested (e.g., an attachment). Bail.
            return !$(this).parents('.view').size();
          })
          .each(function() {
            $(this)
              .addClass('views-refresh-processed')
              .append($('<div></div>')
                .each(function () {
                  // Set a reference that will work in subsequent calls.
                  var target = this;
                  var viewData = {};
                  $.extend(
                    viewData,
                    settings
                  );

                  element_settings.submit = viewData;
                  var ajax = new Drupal.ajax(false, target, element_settings);
                  ajax.options.type = 'GET';
                  ajax.beforeSerialize = simpleBeforeSerialize;

                  setInterval(function() {
                    $(target).trigger('refreshRequested');
                  }, 2000);
                })
              );
        }); // $view.filter().each
      }); // .each Drupal.settings.views.ajaxViews
    } // if
  };

})(jQuery);
