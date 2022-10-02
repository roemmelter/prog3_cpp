// (c) by Stefan Roettger, licensed under MIT license

#if (LGL_OPENGL_VERSION < 30)

#ifndef glGenRenderbuffers
#   define glGenRenderbuffers glGenRenderbuffersEXT
#endif
#ifndef glBindRenderbuffer
#   define glBindRenderbuffer glBindRenderbufferEXT
#endif
#ifndef glRenderbufferStorage
#   define glRenderbufferStorage glRenderbufferStorageEXT
#endif
#ifndef glDeleteRenderbuffers
#   define glDeleteRenderbuffers glDeleteRenderbuffersEXT
#endif

#ifndef glGenFramebuffers
#   define glGenFramebuffers glGenFramebuffersEXT
#endif
#ifndef glBindFramebuffer
#   define glBindFramebuffer glBindFramebufferEXT
#endif
#ifndef glFramebufferTexture2D
#   define glFramebufferTexture2D glFramebufferTexture2DEXT
#endif
#ifndef glFramebufferRenderbuffer
#   define glFramebufferRenderbuffer glFramebufferRenderbufferEXT
#endif
#ifndef glCheckFramebufferStatus
#   define glCheckFramebufferStatus glCheckFramebufferStatusEXT
#endif
#ifndef glDeleteFramebuffers
#   define glDeleteFramebuffers glDeleteFramebuffersEXT
#endif

#ifndef GL_RENDERBUFFER
#   define GL_RENDERBUFFER GL_RENDERBUFFER_EXT
#endif
#ifndef GL_FRAMEBUFFER
#   define GL_FRAMEBUFFER GL_FRAMEBUFFER_EXT
#endif
#ifndef GL_COLOR_ATTACHMENT0
#   define GL_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0_EXT
#endif
#ifndef GL_DEPTH_ATTACHMENT
#   define GL_DEPTH_ATTACHMENT GL_DEPTH_ATTACHMENT_EXT
#endif
#ifndef GL_FRAMEBUFFER_COMPLETE
#   define GL_FRAMEBUFFER_COMPLETE GL_FRAMEBUFFER_COMPLETE_EXT
#endif
#ifndef GL_FRAMEBUFFER_BINDING
#   define GL_FRAMEBUFFER_BINDING GL_FRAMEBUFFER_BINDING_EXT
#endif
#ifndef GL_TEXTURE_COMPARE_MODE
#   define GL_TEXTURE_COMPARE_MODE GL_TEXTURE_COMPARE_MODE_EXT
#endif

#endif
