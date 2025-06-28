#include <AppCore/App.h>
#include <AppCore/Window.h>
#include <AppCore/Overlay.h>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <thread>

using namespace ultralight;

const char* htmlString(double fps, int frame_count, double elapsed_time);

///
///
///
///

class FPSTestApp : public WindowListener,
                   public ViewListener {
  RefPtr<App> app_;
  RefPtr<Window> window_;
  RefPtr<Overlay> overlay_;
  
  std::chrono::high_resolution_clock::time_point start_time_;
  std::chrono::high_resolution_clock::time_point last_update_;
  int frame_count_;
  double current_fps_;
  bool should_update_;
  
public:
  FPSTestApp() : frame_count_(0), current_fps_(0.0), should_update_(true) {
    app_ = App::Create();

    window_ = Window::Create(app_->main_monitor(), 1200, 800, false, kWindowFlags_Titled);
    window_->SetTitle("Ultralight FPS Test - C++ HTML Generation");

    overlay_ = Overlay::Create(window_, window_->width(), window_->height(), 0, 0);

    start_time_ = std::chrono::high_resolution_clock::now();
    last_update_ = start_time_;

    LoadInitialContent();

    window_->set_listener(this);
    overlay_->view()->set_view_listener(this);

  }

  virtual ~FPSTestApp() {}

  void LoadInitialContent() {
    double elapsed = 0.0;
    overlay_->view()->LoadHTML(htmlString(current_fps_, frame_count_, elapsed));
  }

  void UpdateFPS() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update_);
    
    frame_count_++;
    
    if (duration.count() >= 100) {
      auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time_);
      double elapsed_seconds = total_duration.count() / 1000.0;
      
      double frame_time = duration.count() / 1000.0;
      current_fps_ = 1.0 / (frame_time / (frame_count_ - (frame_count_ - 10)));
      
      if (frame_count_ > 10) {
        current_fps_ = frame_count_ / elapsed_seconds;
      }
      
      overlay_->view()->LoadHTML(htmlString(current_fps_, frame_count_, elapsed_seconds));
      
      last_update_ = now;
      
      std::cout << "FPS: " << std::fixed << std::setprecision(1) << current_fps_ 
                << " | Frames: " << frame_count_ 
                << " | Time: " << std::fixed << std::setprecision(2) << elapsed_seconds << "s" << std::endl;
    }
  }

  virtual void OnClose(ultralight::Window* window) override {
    should_quit_ = true;
  }

  virtual void OnResize(ultralight::Window* window, uint32_t width, uint32_t height) override {
    overlay_->Resize(width, height);
  }

  virtual void OnChangeCursor(ultralight::View* caller, ultralight::Cursor cursor) override {
    window_->SetCursor(cursor);
  }

  void Run() {
    while (!should_quit_) {
      UpdateFPS();
      std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS target
    }
  }
  
private:
  bool should_quit_ = false;
};

const char* htmlString(double fps, int frame_count, double elapsed_time) {
  static std::string html_content;
  std::ostringstream oss;
  
  oss << R"(
<!DOCTYPE html>
<html>
<head>
    <style type="text/css">
        * { 
            margin: 0; 
            padding: 0; 
            box-sizing: border-box;
            -webkit-user-select: none; 
        }
        body { 
            font-family: -apple-system, 'Segoe UI', 'Roboto', Arial, sans-serif; 
            background: linear-gradient(45deg, #ff6b6b, #4ecdc4, #45b7d1, #96ceb4, #ffeaa7, #dda0dd);
            background-size: 400% 400%;
            animation: gradientShift 3s ease infinite;
            height: 100vh;
            display: flex;
            flex-direction: column;
            justify-content: center;
            align-items: center;
            color: white;
            text-shadow: 2px 2px 4px rgba(0,0,0,0.5);
        }
        
        @keyframes gradientShift {
            0% { background-position: 0% 50%; }
            50% { background-position: 100% 50%; }
            100% { background-position: 0% 50%; }
        }
        
        .fps-container {
            text-align: center;
            background: rgba(0,0,0,0.3);
            padding: 40px;
            border-radius: 20px;
            backdrop-filter: blur(10px);
            border: 2px solid rgba(255,255,255,0.2);
            animation: pulse 2s ease-in-out infinite;
        }
        
        @keyframes pulse {
            0%, 100% { transform: scale(1); }
            50% { transform: scale(1.05); }
        }
        
        .fps-display {
            font-size: 72px;
            font-weight: bold;
            margin-bottom: 20px;
            color: #00ff88;
            text-shadow: 0 0 20px rgba(0,255,136,0.5);
        }
        
        .fps-label {
            font-size: 24px;
            margin-bottom: 30px;
            opacity: 0.9;
        }
        
        .stats {
            display: flex;
            justify-content: space-around;
            width: 100%;
            margin-top: 30px;
        }
        
        .stat-item {
            text-align: center;
            padding: 15px;
            background: rgba(255,255,255,0.1);
            border-radius: 10px;
            min-width: 120px;
        }
        
        .stat-value {
            font-size: 28px;
            font-weight: bold;
            color: #ffd700;
        }
        
        .stat-label {
            font-size: 14px;
            opacity: 0.8;
            margin-top: 5px;
        }
        
        .performance-bars {
            width: 100%;
            margin-top: 40px;
        }
        
        .bar-container {
            margin: 10px 0;
            background: rgba(255,255,255,0.1);
            border-radius: 10px;
            overflow: hidden;
            height: 20px;
        }
        
        .bar-fill {
            height: 100%;
            background: linear-gradient(90deg, #ff4757, #ffa502, #2ed573);
            border-radius: 10px;
            transition: width 0.3s ease;
        }
        
        .moving-elements {
            position: absolute;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            pointer-events: none;
            z-index: -1;
        }
        
        .floating-circle {
            position: absolute;
            border-radius: 50%;
            background: rgba(255,255,255,0.1);
            animation: float 6s ease-in-out infinite;
        }
        
        @keyframes float {
            0%, 100% { transform: translateY(0px) rotate(0deg); }
            50% { transform: translateY(-20px) rotate(180deg); }
        }
        
        .circle1 { width: 60px; height: 60px; top: 10%; left: 10%; animation-delay: 0s; }
        .circle2 { width: 80px; height: 80px; top: 20%; right: 15%; animation-delay: 1s; }
        .circle3 { width: 40px; height: 40px; bottom: 20%; left: 20%; animation-delay: 2s; }
        .circle4 { width: 70px; height: 70px; bottom: 10%; right: 10%; animation-delay: 3s; }
    </style>
</head>
<body>
    <div class="moving-elements">
        <div class="floating-circle circle1"></div>
        <div class="floating-circle circle2"></div>
        <div class="floating-circle circle3"></div>
        <div class="floating-circle circle4"></div>
    </div>
    
    <div class="fps-container">
        <div class="fps-display">)" << std::fixed << std::setprecision(1) << fps << R"(</div>
        <div class="fps-label">FRAMES PER SECOND</div>
        
        <div class="stats">
            <div class="stat-item">
                <div class="stat-value">)" << frame_count << R"(</div>
                <div class="stat-label">Total Frames</div>
            </div>
            <div class="stat-item">
                <div class="stat-value">)" << std::fixed << std::setprecision(1) << elapsed_time << R"(s</div>
                <div class="stat-label">Runtime</div>
            </div>
            <div class="stat-item">
                <div class="stat-value">)" << std::fixed << std::setprecision(1) << (fps > 0 ? 1000.0/fps : 0) << R"(ms</div>
                <div class="stat-label">Frame Time</div>
            </div>
        </div>
        
        <div class="performance-bars">
            <div class="bar-container">
                <div class="bar-fill" style="width: )" << std::min(100.0, fps * 100.0 / 60.0) << R"(%;"></div>
            </div>
        </div>
    </div>
</body>
</html>
)";

  html_content = oss.str();
  return html_content.c_str();
}

int main() {
  std::cout << "Starting Ultralight FPS Test..." << std::endl;
  std::cout << "This test measures HTML rendering performance using C++ generated content." << std::endl;
  std::cout << "Press Ctrl+C or close the window to exit." << std::endl << std::endl;
  
  FPSTestApp app;
  app.Run();

  return 0;
}
