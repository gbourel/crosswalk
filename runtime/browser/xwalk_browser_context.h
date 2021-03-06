// Copyright (c) 2013 Intel Corporation. All rights reserved.
// Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef XWALK_RUNTIME_BROWSER_XWALK_BROWSER_CONTEXT_H_
#define XWALK_RUNTIME_BROWSER_XWALK_BROWSER_CONTEXT_H_

#if defined(OS_ANDROID)
#include <string>
#endif

#include <map>
#include <vector>

#include "base/compiler_specific.h"
#include "base/files/file_path.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "components/visitedlink/browser/visitedlink_delegate.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/content_browser_client.h"
#include "xwalk/runtime/browser/xwalk_ssl_host_state_delegate.h"

namespace net {
class URLRequestContextGetter;
}

namespace content {
class DownloadManagerDelegate;
}

namespace visitedlink {
class VisitedLinkMaster;
}

namespace xwalk {

class RuntimeDownloadManagerDelegate;
class RuntimeURLRequestContextGetter;

class XWalkBrowserContext
    : public content::BrowserContext
#if defined(OS_ANDROID)
      , public visitedlink::VisitedLinkDelegate
#endif
{
 public:
  XWalkBrowserContext();
  virtual ~XWalkBrowserContext();

  // Convenience method to returns the XWalkBrowserContext corresponding to the
  // given WebContents.
  static XWalkBrowserContext* FromWebContents(
      content::WebContents* web_contents);

  // BrowserContext implementation.
  base::FilePath GetPath() const override;
  bool IsOffTheRecord() const override;
  content::DownloadManagerDelegate* GetDownloadManagerDelegate() override;
  net::URLRequestContextGetter* GetRequestContext() override;
  net::URLRequestContextGetter* GetRequestContextForRenderProcess(
      int renderer_child_id) override;
  net::URLRequestContextGetter* GetMediaRequestContext() override;
  net::URLRequestContextGetter* GetMediaRequestContextForRenderProcess(
      int renderer_child_id) override;
  net::URLRequestContextGetter* GetMediaRequestContextForStoragePartition(
          const base::FilePath& partition_path,
          bool in_memory) override;
  content::ResourceContext* GetResourceContext() override;
  content::BrowserPluginGuestManager* GetGuestManager() override;
  storage::SpecialStoragePolicy* GetSpecialStoragePolicy() override;
  content::PushMessagingService* GetPushMessagingService() override;
  content::SSLHostStateDelegate* GetSSLHostStateDelegate() override;

  RuntimeURLRequestContextGetter* GetURLRequestContextGetterById(
      const std::string& pkg_id);
  net::URLRequestContextGetter* CreateRequestContext(
      content::ProtocolHandlerMap* protocol_handlers,
      content::URLRequestInterceptorScopedVector request_interceptors);
  net::URLRequestContextGetter* CreateRequestContextForStoragePartition(
      const base::FilePath& partition_path,
      bool in_memory,
      content::ProtocolHandlerMap* protocol_handlers,
      content::URLRequestInterceptorScopedVector request_interceptors);
#if defined(OS_ANDROID)
  void SetCSPString(const std::string& csp);
  std::string GetCSPString() const;
  // These methods map to Add methods in visitedlink::VisitedLinkMaster.
  void AddVisitedURLs(const std::vector<GURL>& urls);
  // visitedlink::VisitedLinkDelegate implementation.
  void RebuildTable(
      const scoped_refptr<URLEnumerator>& enumerator) override;
#endif

 private:
  class RuntimeResourceContext;

  // Performs initialization of the XWalkBrowserContext while IO is still
  // allowed on the current thread.
  void InitWhileIOAllowed();

#if defined(OS_ANDROID)
  // Reset visitedlink master and initialize it.
  void InitVisitedLinkMaster();
#endif

  scoped_ptr<RuntimeResourceContext> resource_context_;
  scoped_refptr<RuntimeDownloadManagerDelegate> download_manager_delegate_;
  scoped_refptr<RuntimeURLRequestContextGetter> url_request_getter_;
#if defined(OS_ANDROID)
  std::string csp_;
  scoped_ptr<visitedlink::VisitedLinkMaster> visitedlink_master_;
#endif

  typedef std::map<base::FilePath::StringType,
      scoped_refptr<RuntimeURLRequestContextGetter> >
      PartitionPathContextGetterMap;
  PartitionPathContextGetterMap context_getters_;
  scoped_ptr<XWalkSSLHostStateDelegate> ssl_host_state_delegate_;

  DISALLOW_COPY_AND_ASSIGN(XWalkBrowserContext);
};

}  // namespace xwalk

#endif  // XWALK_RUNTIME_BROWSER_XWALK_BROWSER_CONTEXT_H_
