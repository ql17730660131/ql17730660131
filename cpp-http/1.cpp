// .Get("/streamed",
//              [&](const Request & /*req*/, Response &res) {
//                res.set_content_provider(
//                    6, "text/plain",
//                    [](size_t offset, size_t /*length*/, DataSink &sink) {
//                      sink.os << (offset < 3 ? "a" : "b");
//                      return true;
//                    });
//              })


//              .Get("/streamed-with-range",
//              [&](const Request & /*req*/, Response &res) {
//                auto data = new std::string("abcdefg");
//                res.set_content_provider(
//                    data->size(), "text/plain",
//                    [data](size_t offset, size_t length, DataSink &sink) {
//                      size_t DATA_CHUNK_SIZE = 4;
//                      const auto &d = *data;
//                      auto out_len =
//                          std::min(static_cast<size_t>(length), DATA_CHUNK_SIZE);
//                      auto ret =
//                          sink.write(&d[static_cast<size_t>(offset)], out_len);
//                      EXPECT_TRUE(ret);
//                      return true;
//                    },
//                    [data](bool success) {
//                      EXPECT_TRUE(success);
//                      delete data;
//                    });
//              })