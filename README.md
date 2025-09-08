<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0" />
  <title>My Portfolio</title>
  <style>
    body {
      font-family: 'Segoe UI', sans-serif;
      margin: 0;
      padding: 0;
      background: #f4f4f4;
      color: #333;
    }
    header {
      background: #222;
      color: #fff;
      padding: 60px 20px;
      text-align: center;
    }
    section {
      padding: 40px 20px;
      max-width: 1000px;
      margin: auto;
    }
    h2 {
      color: #444;
      margin-bottom: 20px;
    }
    .projects {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
      gap: 20px;
    }
    .project-card {
      background: #fff;
      padding: 20px;
      border-radius: 8px;
      box-shadow: 0 2px 5px rgba(0,0,0,0.1);
    }
    footer {
      background: #222;
      color: #fff;
      text-align: center;
      padding: 20px;
    }
    a {
      color: #00ccc2;
      text-decoration: none;
    }
  </style>
</head>
<body>

  <header>
    <h1>Hello, I'm [Ujjwal Dwivedi]</h1>
    <p>Web Developer</p>
  </header>

  <section id="about">
    <h2>About Me</h2>
    <p>I’m passionate about building beautiful and functional websites. With experience in HTML, CSS, JavaScript, and more, I love turning ideas into reality.</p>
  </section>

  <section id="projects">
    <h2>Projects</h2>
    <div class="projects">
      <div class="project-card">
        <h3>Project One</h3>
        <p>A responsive website built with HTML, CSS, and JavaScript.</p>
        <a href="#">View Project</a>
      </div>
      <div class="project-card">
        <h3>Project Two</h3>
        <p>A portfolio site showcasing design and development skills.</p>
        <a href="#">View Project</a>
      </div>
      <!-- Add more project cards as needed -->
    </div>
  </section>

  <section id="contact">
    <h2>9336608743</h2>
    <p>Feel free to reach out via <a href="mailto:your.siddharthdwivedi557@example.com">email</a> or connect on <a href="Ujjwal Dwivedi">LinkedIn</a>.</p>
  </section>

  <footer>
    <p>&copy; 2025 [Ujjwal dwivedi]. All rights reserved.</p>
  </footer>

</body>
</html>
