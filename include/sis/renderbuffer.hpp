#ifndef UMD_RENDERBUFFER_HPP
#define UMD_RENDERBUFFER_HPP

#include "storage.hpp"
#include "identifiable.hpp"
#include "functions.hpp"


namespace botson {
    namespace sdk {
        class renderbuffer : public std::enable_shared_from_this<renderbuffer>, public identifiable {
        private:
            unsigned int _id;


        public:
            explicit renderbuffer(unsigned int id) : _id(id) {};

            unsigned int id() override { return _id; }

            static std::shared_ptr<renderbuffer> create() {
                GLuint rid;
                glGenRenderbuffers(1, &rid);
                return std::make_shared<renderbuffer>(rid);
            }

            std::shared_ptr<renderbuffer> xhis() {
                return shared_from_this();
            };

            std::shared_ptr<renderbuffer> attach(storage object) {
                ensure(glBindRenderbuffer)(GL_RENDERBUFFER, std::forward<GLuint >(_id));
                if (object.units() > 1) {
                    ensure(glRenderbufferStorageMultisample)(GL_RENDERBUFFER, object.units(),
                                                                  GL_RGBA4, object.width(), object.height());
                } else {
                    ensure(glRenderbufferStorage)(GL_RENDERBUFFER, GL_RGBA4, object.width(),
                                                       object.height());
                }
                return xhis();
            }

            unsigned int getSamples() {
                GLint sample;
                glGetRenderbufferParameteriv(_id, GL_RENDERBUFFER_SAMPLES, &sample);
                return sample;
            }
        };
    }
}

#endif //UMD_RENDERBUFFER_HPP
